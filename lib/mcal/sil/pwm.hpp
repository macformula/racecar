#pragma once

#include <algorithm>

#include "periph/pwm.hpp"

namespace mcal::sil {

class PWMOutput : public macfe::periph::PWMOutput {
public:
    explicit PWMOutput(float* output) : output_(output) {}

    void Start() override {}
    void Stop() override {}

    void SetDutyCycle(float duty_cycle) override {
        *output_ = std::clamp(duty_cycle, 0.0f, 100.0f);
    }

    float GetDutyCycle() override {
        return *output_;
    }

    void SetFrequency(float frequency) override {
        frequency_ = frequency;
    }

    float GetFrequency() override {
        return frequency_;
    }

private:
    float* output_;
    float frequency_ = 0.0f;
};

}  // namespace mcal::sil