/// @author Tamara Xu
/// @date 2024-10-03

#pragma once

#include <format>
#include <iostream>
#include <string>

#include "periph/analog_output.hpp"

namespace mcal::cli {

class AnalogOutput : public macfe::periph::AnalogOutput {
public:
    AnalogOutput(std::string name) : name_(name) {}

    void SetVoltage(float voltage) override {
        std::cout << std::format("Setting AnalogOutput {} to {:.3f} V", name_,
                                 voltage)
                  << std::endl;
    }

private:
    std::string name_;
};

}  // namespace mcal::cli
