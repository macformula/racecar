#pragma once

#include <iostream>
#include <map>
#include <string>

#include "lvcontroller.pb.h"
#include "periph/analog_input.hpp"

namespace mcal::sil {

class AnalogInput : public macfe::periph::AnalogInput {
private:
    float* input_;

public:
    AnalogInput(float* input) : input_(input) {}
    float ReadVoltage() {
        return *input_;
    }
};
}  // namespace mcal::sil