/// @author Samuel Shi
/// @date 2024-11-17

#pragma once

#include <iomanip>
#include <iostream>

#include "shared/periph/analog_input.hpp"

namespace mcal::cli::periph {

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    float ReadVoltage() override {
        std::cout << "Reading ADC " << name_ << " ..." << std::endl;
        std::cout << " | Enter a voltage level: ";
        float voltage;
        std::cin >> voltage;
        std::cout << " | Obtained value " << std::fixed << std::setprecision(3)
                  << voltage << " V" << std::endl;
        return voltage;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli::periph
