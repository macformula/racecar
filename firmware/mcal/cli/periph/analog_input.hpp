/// @author Samuel Shi
/// @date 2024-11-17

#pragma once

#include <format>
#include <iostream>

#include "shared/periph/analog_input.hpp"

namespace mcal::cli::periph {

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << std::format("Reading ADC {} ...", name_) << std::endl;
    }

    float Read() override {
        Start();
        float voltage;
        std::cout << " | Enter a voltage level: ";
        std::cin >> voltage;
        std::cout << std::format(" | Obtained value {:.3f} V", voltage)
                  << std::endl;
        return voltage;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli::periph
