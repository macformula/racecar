/// @author Tamara Xu
/// @date 2025-01-16

#pragma once

#include <cstdint>

#include "shared/periph/analog_output.hpp"
#include "shared/util/mappers/clamper.hpp"

// #include "stm32f7xx_hal.h"   // use this for stm32f767
#include "stm32f4xx_hal.h"      // using this for tesing on stm32f446re

namespace mcal::stm32f767::periph {

class AnalogOutputDAC : public shared::periph::AnalogOutput {
public:
    AnalogOutputDAC(DAC_HandleTypeDef* hdac, uint32_t channel)
        : hdac_(hdac), channel_(channel) {}

    void SetVoltage(float voltage) override {
        HAL_DAC_Start(hdac_, channel_);
        
        uint32_t dacValue = shared::util::Clamper<float>::Evaluate(
            (voltage / 3.3f) * 4095, 0, 4095);

        HAL_DAC_SetValue(hdac_, channel_, DAC_ALIGN_12B_R, dacValue);
    }

private:
    DAC_HandleTypeDef* hdac_;
    uint32_t channel_;
};

}  // namespace mcal::stm32f767::periph
