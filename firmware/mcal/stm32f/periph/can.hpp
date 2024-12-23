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

#include "etl/queue_spsc_atomic.h"
#include "shared/comms/can/msg.hpp"
#include "shared/periph/can.hpp"

namespace mcal::stm32f::periph {

namespace priv {
class InterruptHandler;
}  // namespace priv

class CanBase : public shared::periph::CanBase {
public:
    CanBase(CAN_HandleTypeDef* hcan);

    void Setup();
    void Send(const shared::can::RawMessage& can_tx_msg) override;

private:
    uint32_t GetTimestamp() const override;
    void ConfigFilters();
    void Receive();
    void SendLL(const shared::can::RawMessage& msg);

    CAN_HandleTypeDef* hcan_;

    /// The STM32F7 CAN peripheral only has 3 Tx mailboxes. This queue provides
    /// more space to ensure bursts of `.Send()` don't overflow them.
    etl::queue_spsc_atomic<shared::can::RawMessage, 16> tx_queue_;

    /// @todo broadcast these over a can message
    uint32_t dropped_tx_frames_ = 0;

    friend class priv::InterruptHandler;
};

}  // namespace mcal::stm32f::periph

#endif  // HAL_CAN_MODULE_ENABLED