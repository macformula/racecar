#include <cstdint>
#include <iostream>

#include "adc.h"

namespace mcal::linux::periph {

ADCInput::ADCInput(std::string name) : name_(name) {}

ADCInput::Start() {
    std::cout << "Reading ADC \"" << name_ << "\"" << std::endl;
}

ADCInput::Read() {
    Start();

    uint32_t adc_val;
    std::cout << " | Enter an unsigned 32-bit value: ";
    std::cin >> adc_val;
    std::cout << " | Obtained value " << adc_val << std::endl;

    return adc_val;
}

}  // namespace mcal::linux::periph