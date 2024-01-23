/// @author Blake Freer
/// @date 2023-11-09

#ifndef MCAL_STM32F767_PERIPH_ADC_H_
#define MCAL_STM32F767_PERIPH_ADC_H_

#include <cstdint>

#include "shared/util/peripheral.h"
#include "stm32f7xx_hal.h"

namespace mcal::periph {

class ADCInput : public shared::util::Peripheral {
private:
    ADC_HandleTypeDef* hadc_;
    uint32_t adc_channel_;

public:
    ADCInput(ADC_HandleTypeDef* hadc, uint32_t adc_channel)
        : hadc_(hadc), adc_channel_(adc_channel){};

    void Start() {
        ADC_ChannelConfTypeDef adc_config = {
            .Channel = adc_channel_,
            .Rank = ADC_REGULAR_RANK_1,
            .SamplingTime = ADC_SAMPLETIME_28CYCLES,
            .Offset = 0};

        HAL_ADC_ConfigChannel(hadc_, &adc_config);
        HAL_ADC_Start(hadc_);
    }

    uint32_t Read() {
        /// @todo should there be a standard output range?
        /// if so, the conversion would require knowing the resolution and
        /// alignment settings

        /// @todo change this to be non-blocking, currently just for demo
        Start();
        HAL_ADC_PollForConversion(hadc_, 1000);
        uint32_t adc_value = HAL_ADC_GetValue(hadc_);
        HAL_ADC_Stop(hadc_);

        return adc_value;
    }
};

}  // namespace mcal::periph

#endif