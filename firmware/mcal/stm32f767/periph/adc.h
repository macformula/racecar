/// @author Blake Freer
/// @date 2023-11-09

#ifndef MCAL_STM32F767_PERIPH_ADC_H_
#define MCAL_STM32F767_PERIPH_ADC_H_

#include <cstdint>
#include "stm32f7xx_hal.h"

#include "shared/util/peripheral.h"

namespace mcal::periph {

class ADCInput : public shared::util::Peripheral {

private:
	ADC_HandleTypeDef* hadc_;

public:
	ADCInput(ADC_HandleTypeDef* hadc) : hadc_(hadc) {};
	
	void Start() {
		HAL_ADC_Start(hadc_);
	}
	
	std::uint32_t Read() {
		/// @todo should there be a standard output range?
		/// if so, the conversion would require knowing the resolution and
		/// alignment settings

		/// @todo change this to be non-blocking, currently just for demo
		Start();
		while(HAL_ADC_PollForConversion(hadc_, 1000) != HAL_OK);
		return HAL_ADC_GetValue(hadc_);
	}
};

} // namespace mcal::periph

#endif