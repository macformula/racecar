#pragma once

#include <fmt/core.h>

#include <iostream>

#include "lvcontroller.pb.h"
#include "periph/pwm.hpp"

namespace mcal::sil {

class PWMOutput : public macfe::periph::PWMOutput {
private:
    lvcontroller_PWM* output_struct_;

public:
    PWMOutput(lvcontroller_PWM* output_struct)
        : output_struct_(output_struct) {}
    void Start() override {
        std::cout << "Starting PWM" << std::endl;
    }
    void Stop() override {
        std::cout << "Stopping PWM" << std::endl;
    }
    void SetDutyCycle(float duty_cycle) override {
        output_struct_->duty_cycle =
            std::max<float>(0, std::min<float>(100, duty_cycle));
    }
    float GetDutyCycle() override {
        return output_struct_->duty_cycle;
    }
    void SetFrequency(float frequency) override {
        output_struct_->frequency_hz = std::max<float>(0, frequency);
    }
    float GetFrequency() override {
        return output_struct_->frequency_hz;
    }
};
}  // namespace mcal::sil