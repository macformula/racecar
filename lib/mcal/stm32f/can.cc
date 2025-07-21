#include "main.h"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED

#include <cstdint>

#include "can.hpp"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#endif

using CanBase = mcal::stm32f::CanBase;

namespace interrupt_handler {

// Store references to the CanBase so we know where to send
CanBase* can1 = nullptr;
CanBase* can2 = nullptr;

#ifdef STM32F7  // CAN3 is available on F7, not F4
CanBase* can3 = nullptr;
#endif

CanBase* GetCanBase(CAN_HandleTypeDef* hcan) {
    if (hcan->Instance == CAN1) {
        return can1;
    } else if (hcan->Instance == CAN2) {
        return can2;
#ifdef STM32F7
    } else if (hcan->Instance == CAN3) {
        return can3;
#endif
    } else {
        return nullptr;
    }
}

void RegisterCanBase(CAN_HandleTypeDef* hcan, CanBase* can_base) {
    if (hcan->Instance == CAN1) {
        can1 = can_base;
    } else if (hcan->Instance == CAN2) {
        can2 = can_base;
#ifdef STM32F7
    } else if (hcan->Instance == CAN3) {
        can3 = can_base;
#endif
    } else {
        // unknown handle
    }
}

}  // namespace interrupt_handler

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    // Overrides the "weak" callback defined by CubeMX
    CanBase* can_base = interrupt_handler::GetCanBase(hcan);
    if (can_base != nullptr) {
        can_base->Receive();
    }
}

#ifdef USE_FREERTOS
#define IMPLEMENT_CAN_TX_CB(CALLBACK)                            \
    void CALLBACK(CAN_HandleTypeDef* hcan) {                     \
        CanBase* can_base = interrupt_handler::GetCanBase(hcan); \
        if (can_base != nullptr) {                               \
            can_base->UnblockTxTask();                           \
        }                                                        \
    }

// Identically implement all three callbacks since the message could have gone
// through any mailbox
IMPLEMENT_CAN_TX_CB(HAL_CAN_TxMailbox0CompleteCallback);
IMPLEMENT_CAN_TX_CB(HAL_CAN_TxMailbox1CompleteCallback);
IMPLEMENT_CAN_TX_CB(HAL_CAN_TxMailbox2CompleteCallback);
#endif
}

namespace mcal::stm32f {

CanBase::CanBase(CAN_HandleTypeDef* hcan) : hcan_{hcan} {}

void CanBase::Setup() {
    interrupt_handler::RegisterCanBase(hcan_, this);
    ConfigFilters();

#if USE_FREERTOS
    // Create the FreeRTOS task for CAN transmitting
    tx_task = xTaskCreateStatic(TxTask, "CanTxTask", TX_TASK_STACK_SIZE, this,
                                TX_TASK_PRIORITY, tx_task_stack,
                                &tx_task_control_block);
    tx_queue = xQueueCreateStatic(TX_QUEUE_SIZE, sizeof(macfe::can::RawMessage),
                                  tx_queue_storage, &tx_queue_static);

    // All mailboxes starts empty
    empty_mailbox = xSemaphoreCreateCountingStatic(
        NUM_TX_MAILBOXES, NUM_TX_MAILBOXES, &empty_mailbox_static);
    HAL_CAN_ActivateNotification(hcan_, CAN_IT_TX_MAILBOX_EMPTY);
#endif

    HAL_CAN_ActivateNotification(hcan_, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_Start(hcan_);
}

#ifdef USE_FREERTOS
void CanBase::Send(const macfe::can::RawMessage& msg) {
    if (xQueueSendToBack(tx_queue, &msg, 0) == errQUEUE_FULL) {
        dropped_tx_frames_++;
    }
}

void CanBase::UnblockTxTask(void) {
    BaseType_t woken_task = pdFALSE;
    xSemaphoreGiveFromISR(empty_mailbox, &woken_task);
    portYIELD_FROM_ISR(woken_task);
}

void CanBase::TxTask(void* _can_base) {
    CanBase* can_base = (CanBase*)_can_base;

    macfe::can::RawMessage msg;

    while (true) {
        // Wait for a message to arrive
        if (xQueueReceive(can_base->tx_queue, &msg, portMAX_DELAY) == pdFALSE) {
            continue;  // should not timeout if messages are being sent
        }

        // Wait for an empty TX mailbox. Short timeout since messages should
        // send fast.
        if (xSemaphoreTake(can_base->empty_mailbox, pdMS_TO_TICKS(10)) ==
            pdFALSE) {
            CanErrorHandler(can_base);

            // Still need to wait for a mailbox to come available
            while (xSemaphoreTake(can_base->empty_mailbox, portMAX_DELAY) ==
                   pdFALSE) {
                continue;
            }
        }
        can_base->SendLL(msg);
    }
}

void CanBase::SendLL(const macfe::can::RawMessage& msg) {
#else

void CanBase::Send(const macfe::can::RawMessage& msg) {
#endif
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

    uint32_t tick = HAL_GetTick();
    while (HAL_CAN_AddTxMessage(hcan_, &stm_tx_header, msg.data,
                                &tx_mailbox_addr_) != HAL_OK) {
        // Attempt to send the message for up to 3ms (blocking)
        // This is bad code.
        // See description at https://github.com/macformula/racecar/pull/480
#if USE_FREERTOS
        dropped_tx_frames_++;
#else
        if (HAL_GetTick() - tick >= 3) {
            dropped_tx_frames_ += 1;
            return;
        }
#endif
    }
}

void CanBase::Receive() {
    macfe::can::RawMessage rx_msg;

    CAN_RxHeaderTypeDef stm_rx_header;
    HAL_CAN_GetRxMessage(hcan_, CAN_RX_FIFO0, &stm_rx_header, rx_msg.data);

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
        .FilterFIFOAssignment = CAN_RX_FIFO0,
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

}  // namespace mcal::stm32f

#endif  // HAL_CAN_MODULE_ENABLED
