#include "stm32f7xx_hal.h"
#ifdef HAL_CAN_MODULE_ENABLED

#include <cstdint>

#include "can.hpp"

// FIFO0 is the "high priority" queue on stm32f7. We do not use FIFO1
constexpr uint32_t kCanFifo = CAN_RX_FIFO0;
constexpr uint32_t kCanInterrupt = CAN_IT_RX_FIFO0_MSG_PENDING;

namespace {  // InterruptHandler is private to this file

/// Prior to this class, we spent several hours debugging an issue where the
/// interrupt was activated but the handler was not being called, causing the
/// program to get repeatedly executed the interupt callback and stall the rest
/// of the program.
/// This class ensures that interrupts activated and handled together.
class InterruptHandler {
    using CanBase = mcal::stm32f767::periph::CanBase;

public:
    void RegisterCanBase(CAN_HandleTypeDef* hcan, CanBase* can_base) {
        int index = HandleToIndex(hcan);
        if (index == -1) return;

        can_bases[index] = can_base;
        HAL_CAN_ActivateNotification(hcan, kCanInterrupt);
    }

    void Handle(CAN_HandleTypeDef* hcan) {
        int index = HandleToIndex(hcan);
        if (index == -1) return;

        auto can_base = can_bases[index];
        if (can_base != nullptr) {
            can_bases[index]->Receive();
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
        } else if (hcan->Instance == CAN3) {
            return 2;
        } else {
            return -1;
        }
    }
};
}  // namespace

static InterruptHandler interrupt_handler;

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    // Overrides the "weak" callback defined by CubeMX
    interrupt_handler.Handle(hcan);
}
}

namespace mcal::stm32f767::periph {

CanBase::CanBase(CAN_HandleTypeDef* hcan) : hcan_{hcan} {}

void CanBase::Setup() {
    ConfigFilters();
    interrupt_handler.RegisterCanBase(hcan_, this);
    HAL_CAN_Start(hcan_);
}

void CanBase::Send(const shared::can::RawMessage& msg) {
    uint32_t tx_mailboxes_free_level = HAL_CAN_GetTxMailboxesFreeLevel(hcan_);
    if (tx_mailboxes_free_level < 1) {
        dropped_tx_frames_ += 1;
        return;
    }

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
    return HAL_GetTick() * HAL_GetTickFreq();
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

    HAL_CAN_ConfigFilter(hcan_, &filter_config);
}

}  // namespace mcal::stm32f767::periph

#endif  // HAL_CAN_MODULE_ENABLED