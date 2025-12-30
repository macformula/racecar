/// @author Samuel Shi
/// @date 2025-1-10

#include "analog_input.hpp"

#include <fmt/core.h>
#include <iostream>

namespace mcal::lnx {

AnalogInput::AnalogInput(std::string name) : name_(name) {}

float AnalogInput::ReadVoltage() {
    std::cout << fmt::format("Reading ADC {} ...", name_) << std::endl;
    std::cout << " | Enter a voltage level: ";
    float voltage;
    std::cin >> voltage;
    std::cout << fmt::format(" | Obtained value {:.3f} V", voltage)
              << std::endl;
    return voltage;
}

};  // namespace mcal::lnx
