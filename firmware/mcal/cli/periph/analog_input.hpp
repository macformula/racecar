/// @author Samuel Shi 
/// @date 2024-11-17

#pragma once

#include <iostream>
#include "shared/periph/analog_input.hpp"
#include <format>

namespace mcal::cli::periph{

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << "Reading ADC " << name_ << "..." << std::endl;
    }

    float Read() override {
        Start();
        float voltage;
        std::cout << " | Enter a voltage level: ";
        std::cin >> voltage;
        std::cout << std::format( " | Obtained value {:.3f}",voltage )  << std::endl;
        return  voltage;
    }
    private:
        std::string name_;
};

}
