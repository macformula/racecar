/// @author Tamara Xu
/// @date 2024-10-13

#pragma once

#include <cstdint>

#include "shared/periph/analog_output.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "stm32f7xx_hal.h"

namespace mcal::stm32f767::periph {

class AnalogOutputPWM : public shared::periph::AnalogOutput {
public:
    AnalogOutputPWM(TIM_HandleTypeDef* htim, uint32_t channel)
        : htim_(htim), channel_(channel) {}

    void SetVoltage(float voltage) override {
        uint32_t maxPulse = __HAL_TIM_GetAutoreload(htim_);
        uint32_t pulse = shared::util::Clamper<float>::Evaluate(
            (voltage * maxPulse) / 3.3f, 0, maxPulse);

        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;
};

}  // namespace mcal::stm32f767::periph
