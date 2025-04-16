#pragma once

#include <cstdint>

#include "shared/periph/analog_input.hpp"

#ifdef STM32F7
#include "stm32f7xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#define ADC_REGULAR_RANK_1                                                    \
    ((uint32_t)0x00000001) /*!< ADC regular conversion rank 1 (not defined in \
                              stm32f4) */
#endif

#ifdef HAL_ADC_MODULE_ENABLED

namespace mcal::stm32f::periph {

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(ADC_HandleTypeDef* hadc, uint32_t adc_channel,
                float system_volts = 3.3f)
        : hadc_(hadc), adc_channel_(adc_channel), system_volts_(system_volts){};

    float ReadVoltage() override {
        Start();
        HAL_ADC_PollForConversion(hadc_, 1000);
        uint32_t adc_value = HAL_ADC_GetValue(hadc_);
        HAL_ADC_Stop(hadc_);

        return adc_value / GetDivisor() * system_volts_;
    }

private:
    ADC_HandleTypeDef* hadc_;
    uint32_t adc_channel_;
    const float system_volts_;

    void Start() {
        ADC_ChannelConfTypeDef adc_config = {
            .Channel = adc_channel_,
            .Rank = ADC_REGULAR_RANK_1,
            .SamplingTime = ADC_SAMPLETIME_28CYCLES,
            .Offset = 0};

        HAL_ADC_ConfigChannel(hadc_, &adc_config);
        HAL_ADC_Start(hadc_);
    }

    float GetDivisor() {
        switch (ADC_GET_RESOLUTION(hadc_)) {
            case ADC_RESOLUTION_6B:
                return 63.0f;
            case ADC_RESOLUTION_8B:
                return 255.0f;
            case ADC_RESOLUTION_10B:
                return 1023.0f;
            case ADC_RESOLUTION_12B:
                return 4095.0f;
            default:
                return 4095.0f;
        }
    };
};

}  // namespace mcal::stm32f::periph

#endif  // HAL_ADC_MODULE_ENABLED