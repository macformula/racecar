/// @author Blake Freer
/// @date 2023-11-09

#ifndef MCAL_STM32F767_ADC_ADC_INPUT_H_
#define MCAL_STM32F767_ADC_ADC_INPUT_H_

#include "stdint.h"
#include "stm32f7xx_hal_adc.h"
#include "infra/adc/adc_input.h"

namespace mcal {
namespace adc {

class ADCInput : public infra::adc::ADCInput {
ADC_HandleTypeDef* hadc_;
public:
	ADCInput(ADC_HandleTypeDef* hadc) : hadc_(hadc) {};
	void Start() noexcept override {
		HAL_ADC_Start(hadc_);
	}
	uint32_t Read() override {
		/// @todo should there be a standard output range?
		/// if so, the conversion would require knowing the resolution and
		/// alignment settings

		/// @todo change this to be non-blockin, just for demo
		Start();
		while(HAL_ADC_PollForConversion(hadc_, 1000) != HAL_OK);
		return HAL_ADC_GetValue(hadc_);
	}
};

} // namespace adc
} // namespace mcal

#endif