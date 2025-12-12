#pragma once

#include "periph/analog_output.hpp"

namespace mcal::sil {

class AnalogOutput : public macfe::periph::AnalogOutput {
private:
    float* output_;

public:
    AnalogOutput(float* output) : output_(output) {}
    void SetVoltage(float voltage) override {
        *output_ = voltage;
    }
};
}  // namespace mcal::sil