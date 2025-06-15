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

namespace mcal::stm32f {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(CAN_HandleTypeDef* hcan);

    void Setup();
    void Send(const shared::can::RawMessage& can_tx_msg) override;
    void Receive();

private:
    uint32_t GetTimestamp() const override;
    void ConfigFilters();

    /// @todo broadcast these over a can message
    uint32_t dropped_tx_frames_ = 0;

    CAN_HandleTypeDef* hcan_;
    uint32_t tx_mailbox_addr_;
};

}  // namespace mcal::stm32f

#endif  // HAL_CAN_MODULE_ENABLED