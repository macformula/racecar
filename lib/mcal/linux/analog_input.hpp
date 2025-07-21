/// @author Samuel Shi
/// @date 2025-1-10

#pragma once

#include <iostream>

#include "periph/analog_input.hpp"

namespace mcal::lnx {

class AnalogInput : public macfe::periph::AnalogInput {
public:
    AnalogInput(std::string name);

    float ReadVoltage() override;

private:
    std::string name_;
};

}  // namespace mcal::lnx
