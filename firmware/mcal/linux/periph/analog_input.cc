/// @author Samuel Shi
/// @date 2025-1-10

#include "analog_input.hpp"

#include <format>
#include <iostream>

namespace mcal::lnx::periph {

AnalogInput::AnalogInput(std::string name) : name_(name) {}

void AnalogInput::Start() {
    std::cout << std::format("Reading ADC {} ...", name_) << std::endl;
}

float AnalogInput::Read() {
    Start();
    float voltage;
    std::cout << " | Enter a voltage level: ";
    std::cin >> voltage;
    std::cout << std::format(" | Obtained value {:.3f} V", voltage)
              << std::endl;
    return voltage;
}

};  // namespace mcal::lnx::periph
