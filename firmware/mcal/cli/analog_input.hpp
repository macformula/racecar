/// @author Samuel Shi
/// @date 2024-11-17

#pragma once

#include <format>
#include <iostream>

#include "shared/periph/analog_input.hpp"

namespace mcal::cli {

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    float ReadVoltage() override {
        std::cout << std::format("Reading ADC {} ...", name_) << std::endl;
        std::cout << " | Enter a voltage level: ";
        float voltage;
        std::cin >> voltage;
        std::cout << std::format(" | Obtained value {:.3f} V", voltage)
                  << std::endl;
        return voltage;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli
