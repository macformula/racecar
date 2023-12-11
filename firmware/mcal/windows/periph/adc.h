/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_ADC_H_
#define MCAL_WINDOWS_PERIPH_ADC_H_

#include <cstdint>
#include <iostream>

#include "shared/util/peripheral.h"

namespace mcal::periph {

class ADCInput : public shared::util::Peripheral {
public:
	ADCInput(int channel) : channel_(channel) {

	}

	void Start() {
		std::cout << "Reading ADC Channel " << channel_ << "..." << std::endl;
	}

	uint32_t Read() {
		Start();
		uint32_t adc_val;
		std::cout << " | Enter an unsigned 32-bit value: ";
		std::cin >> adc_val;
		std::cout << " | Obtained value " << adc_val << std::endl;
		return adc_val; 
	}

private:
	int channel_;
};

} // namespace mcal::periph

#endif