/// @author Samuel Shi
/// @date 2024-11-17

#pragma once

#include <fmt/core.h>
#include <iostream>

#include "periph/analog_input.hpp"

namespace mcal::cli {

class AnalogInput : public macfe::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    float ReadVoltage() override {
        std::cout << fmt::format("Reading ADC {} ...", name_) << std::endl;
        std::cout << " | Enter a voltage level: ";
        float voltage;
        std::cin >> voltage;
        std::cout << fmt::format(" | Obtained value {:.3f} V", voltage)
                  << std::endl;
        return voltage;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli
