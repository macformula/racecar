/// @author Blake Freer
/// @date 2024-11

#pragma once

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED

#include <cstdint>

#include "shared/comms/can/msg.hpp"
#include "shared/periph/can.hpp"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#endif

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#endif

namespace mcal::stm32f {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(CAN_HandleTypeDef* hcan);

    void Setup();
    void Send(const shared::can::RawMessage& can_tx_msg) override;
    void Receive();

private:
    static const uint8_t NUM_TX_MAILBOXES = 3;

    uint32_t GetTimestamp() const override;
    void ConfigFilters();

    /// @todo broadcast these over a can message
    uint32_t dropped_tx_frames_ = 0;

    CAN_HandleTypeDef* hcan_;
    uint32_t tx_mailbox_addr_;

#ifdef USE_FREERTOS
public:
    void UnblockTxTask(void);

private:
    static void TxTask(void* can_base);
    void SendLL(const shared::can::RawMessage& can_tx_msg);

    static const uint32_t TX_TASK_PRIORITY = 5;  // should be highest priority
    static const size_t TX_TASK_STACK_SIZE = 256;
    TaskHandle_t tx_task;
    StaticTask_t tx_task_control_block;
    StackType_t tx_task_stack[TX_TASK_STACK_SIZE];

    // Buffer messages in a queue so the app can send many at once without
    // overflowing the mailboxes
    static const size_t TX_QUEUE_SIZE = 30;
    QueueHandle_t tx_queue;
    StaticQueue_t tx_queue_static;
    uint8_t tx_queue_storage[sizeof(shared::can::RawMessage) * TX_QUEUE_SIZE];

    // Use a semaphore to guard the TX mailboxes
    SemaphoreHandle_t empty_mailbox;
    StaticSemaphore_t empty_mailbox_static;
#endif
};

}  // namespace mcal::stm32f

#endif  // HAL_CAN_MODULE_ENABLED
