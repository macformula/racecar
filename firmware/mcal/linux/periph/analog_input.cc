/// @author Samuel Shi
/// @date 2025-1-10

#include <format>
#include <iostream>

#include "analog_input.hpp"


namespace mcal::lnx::periph {

AnalogInput::AnalogInput(std::string name) : name_(name) {}

float AnalogInput::ReadVoltage() {
    std::cout << std::format("Reading ADC {} ...", name_) << std::endl;
    std::cout << " | Enter a voltage level: ";
    float voltage;
    std::cin >> voltage;
    std::cout << std::format(" | Obtained value {:.3f} V", voltage)
              << std::endl;
    return voltage;
}

};  // namespace mcal::lnx::periph
