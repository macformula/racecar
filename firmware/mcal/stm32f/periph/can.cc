#include "main.h"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED

#include <cstdint>

#include "can.hpp"
#include "etl/queue_spsc_atomic.h"

// FIFO0 is the "high priority" queue on stm32f7. We do not use FIFO1
constexpr uint32_t kCanFifo = CAN_RX_FIFO0;
constexpr uint32_t kCanInterrupt = CAN_IT_RX_FIFO0_MSG_PENDING;

namespace mcal::stm32f767::periph::priv {

/// Prior to this class, we spent several hours debugging an issue where the
/// interrupt was activated but the handler was not being called, causing the
/// program to get repeatedly executed the interupt callback and stall the rest
/// of the program.
/// This class ensures that interrupts activated and handled together.
class InterruptHandler {
    using CanBase = mcal::stm32f::periph::CanBase;

public:
    void RegisterCanBase(CAN_HandleTypeDef* hcan, CanBase* can_base) {
        int index = HandleToIndex(hcan);
        if (index == -1) return;

        can_bases[index] = can_base;
        HAL_CAN_ActivateNotification(hcan, kCanInterrupt);
        HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY);
    }

    void Receive(CAN_HandleTypeDef* hcan) {
        auto can_base = HandleToCanBase(hcan);
        if (can_base == nullptr) return;

        can_base->Receive();
    }

    void LoadTx(CAN_HandleTypeDef* hcan) {
        auto can_base = HandleToCanBase(hcan);
        if (can_base == nullptr) return;

        shared::can::RawMessage msg;
        auto msg_loaded = can_base->tx_queue_.pop(msg);
        if (msg_loaded) {
            can_base->SendLL(msg);
        }
    }

private:
    CanBase* can_bases[3] = {nullptr, nullptr, nullptr};

    int HandleToIndex(CAN_HandleTypeDef* hcan) {
        // CANx are not contiguous in memory, so we can't use a simple array
        if (hcan->Instance == CAN1) {
            return 0;
        } else if (hcan->Instance == CAN2) {
            return 1;
#ifdef STM32F7  // CAN3 is available on F7, not F4
        } else if (hcan->Instance == CAN3) {
            return 2;
#endif
        } else {
            return -1;
        }
    }

    CanBase* HandleToCanBase(CAN_HandleTypeDef* hcan) {
        int index = HandleToIndex(hcan);
        if (index == -1) return nullptr;

        return can_bases[index];
    }
};
}  // namespace mcal::stm32f767::periph::priv

// Define the HAL Interrupt callbacks, overriding the "weak" callbacks
// defined by CubeMX. By statically declaring the callbacks, the firmware
// applications can "just use" CanBase without needing to set up interrupts.
static mcal::stm32f767::periph::priv::InterruptHandler interrupt_handler;

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    // Message was received
    interrupt_handler.Receive(hcan);
}

// All tx empty callbacks are the same.
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan) {
    interrupt_handler.LoadTx(hcan);
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef* hcan) {
    interrupt_handler.LoadTx(hcan);
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef* hcan) {
    interrupt_handler.LoadTx(hcan);
}
}

namespace mcal::stm32f::periph {

CanBase::CanBase(CAN_HandleTypeDef* hcan) : hcan_{hcan} {}

void CanBase::Setup() {
    ConfigFilters();
    interrupt_handler.RegisterCanBase(hcan_, this);
    HAL_CAN_Start(hcan_);
}

void CanBase::Send(const shared::can::RawMessage& msg) {
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan_) > 0) {
        SendLL(msg);
    } else {
        bool msg_dropped = !tx_queue_.push(msg);
        if (msg_dropped) {  // Queue is full
            dropped_tx_frames_ += 1;
        }
    }
}

void CanBase::SendLL(const shared::can::RawMessage& msg) {
    CAN_TxHeaderTypeDef stm_tx_header;
    stm_tx_header.RTR = CAN_RTR_DATA;  // we only support data frames currently
    stm_tx_header.DLC = msg.data_length;

    if (msg.is_extended_frame) {
        stm_tx_header.IDE = CAN_ID_EXT;
        stm_tx_header.ExtId = msg.id;
    } else {
        stm_tx_header.IDE = CAN_ID_STD;
        stm_tx_header.StdId = msg.id;
    }

    uint32_t tx_mailbox_addr_;  // Which mailbox the msg is placed in. Unused.
    HAL_CAN_AddTxMessage(hcan_, &stm_tx_header, msg.data, &tx_mailbox_addr_);
}

void CanBase::Receive() {
    shared::can::RawMessage rx_msg;

    CAN_RxHeaderTypeDef stm_rx_header;
    HAL_CAN_GetRxMessage(hcan_, kCanFifo, &stm_rx_header, rx_msg.data);

    rx_msg.is_extended_frame = stm_rx_header.IDE == CAN_ID_EXT;
    rx_msg.data_length = static_cast<uint8_t>(stm_rx_header.DLC);
    if (rx_msg.is_extended_frame) {
        rx_msg.id = stm_rx_header.ExtId;
    } else {
        rx_msg.id = stm_rx_header.StdId;
    }

    AddToBus(rx_msg);
}

inline uint32_t CanBase::GetTimestamp() const {
    return HAL_GetTick();
}

void CanBase::ConfigFilters() {
    // Currently we don't support filtering - all messages are accepted
    CAN_FilterTypeDef filter_config{
        .FilterIdHigh = 0x0000,
        .FilterIdLow = 0x0000,
        .FilterMaskIdHigh = 0x0000,
        .FilterMaskIdLow = 0x0000,
        .FilterFIFOAssignment = kCanFifo,
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_32BIT,
        .FilterActivation = CAN_FILTER_ENABLE,
        .SlaveStartFilterBank = 14,
    };
    if (hcan_->Instance == CAN2) {
        filter_config.FilterBank = 14;
    }
    HAL_CAN_ConfigFilter(hcan_, &filter_config);
}

}  // namespace mcal::stm32f::periph

#endif  // HAL_CAN_MODULE_ENABLED