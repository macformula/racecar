#include <cstdint>
#include <iostream>

#include "adc.h"

namespace mcal::lnx::periph {

ADCInput::ADCInput(std::string name) : name_(name) {}

void ADCInput::Start() {
    std::cout << "Reading ADC \"" << name_ << "\"" << std::endl;
}

uint32_t ADCInput::Read() {
    Start();

    uint32_t adc_val;
    std::cout << " | Enter an unsigned 32-bit value: ";
    std::cin >> adc_val;
    std::cout << " | Obtained value " << adc_val << std::endl;

    return adc_val;
}

}  // namespace mcal::lnx::periph