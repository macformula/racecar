/// @author Tamara Xu
/// @date 2024-10-13

#pragma once

#include <cstdint>

#include "etl/algorithm.h"
#include "periph/analog_output.hpp"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED

namespace mcal::stm32f {

class AnalogOutputPWM : public macfe::periph::AnalogOutput {
public:
    AnalogOutputPWM(TIM_HandleTypeDef* htim, uint32_t channel)
        : htim_(htim), channel_(channel) {}

    void SetVoltage(float voltage) override {
        uint32_t maxPulse = __HAL_TIM_GetAutoreload(htim_);
        uint32_t pulse =
            etl::clamp<float>((voltage * maxPulse) / 3.3f, 0, maxPulse);

        __HAL_TIM_SetCompare(htim_, channel_, pulse);
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;
};

}  // namespace mcal::stm32f

#endif  // HAL_DAC_MODULE_ENABLED