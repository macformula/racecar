/// @author Tamara Xu
/// @date 2024-10-13

#pragma once

#include <cstdint>

#include "shared/periph/analog_output.h"
#include "shared/util/mappers/clamper.h"
#include "stm32f7xx_hal.h"

namespace mcal::stm32f767::periph {

class AnalogOutput : public shared::periph::AnalogOutput {
public:
    AnalogOutput(TIM_HandleTypeDef* htim, uint32_t channel)
        : htim_(htim), channel_(channel) {}

    void SetVoltage (float voltage) override {
        voltage_ = shared::util::Clamper<float>::Evaluate(voltage, 0, 3.3);
        uint32_t pulse = (voltage_ * float(__HAL_TIM_GetAutoreload(htim_))) /
                 (3.3 * 100.0f);

        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;
    float voltage_;
};

}  // namespace mcal::stm32f767::periph
