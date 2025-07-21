/// @author Tamara Xu
/// @date 2025-01-16

#pragma once

#include <cstdint>

#include "etl/algorithm.h"
#include "periph/analog_output.hpp"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#endif

namespace mcal::stm32f {

class AnalogOutputDAC : public macfe::periph::AnalogOutput {
public:
    AnalogOutputDAC(DAC_HandleTypeDef* hdac, uint32_t channel)
        : hdac_(hdac), channel_(channel) {}

    void SetVoltage(float voltage) override {
        HAL_DAC_Start(hdac_, channel_);

        uint32_t dacValue = etl::clamp<float>((voltage / 3.3f) * 4095, 0, 4095);

        HAL_DAC_SetValue(hdac_, channel_, DAC_ALIGN_12B_R, dacValue);
    }

private:
    DAC_HandleTypeDef* hdac_;
    uint32_t channel_;
};

}  // namespace mcal::stm32f
