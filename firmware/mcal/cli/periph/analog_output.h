/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include <iostream>
#include <string>

#include "shared/periph/analog_output.h"

namespace mcal::cli::periph {

class AnalogOutput : public shared::periph::AnalogOutput {
public:
    AnalogOutput(std::string name) : name_(name) {}

    void SetVoltage (float voltage) override {
        std::cout << "Setting AnalogOutput " << name_ << " to " << voltage << " V" 
                  << std::endl;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli::periph