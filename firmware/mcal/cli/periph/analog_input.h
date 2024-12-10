/// @author Samuel Shi 
/// @date 2024-11-17

#pragma once

#include <iostream>
#include "shared/periph/analog_input.h"

namespace mcal::cli::periph{

class AnalogInput : public shared::periph::AnalogInput {
public:
    AnalogInput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << "Reading ADC " << name_ << "..." << std::endl;
    }

    float Read() override {
        Start();
        float adc_val;
        std::cout << " | Enter an unsigned 32-bit value: ";
        std::cin >> adc_val;
        std::cout << " | Obtained value " <<  (adc_val/4095.0f * 3.3f) << std::endl;
        return adc_val/4095.0f * 3.3f;
    }
    private:
        std::string name_;
};

}
