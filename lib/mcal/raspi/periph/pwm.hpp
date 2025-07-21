/// @author Samuel Parent
/// @date 2023-05-21

#pragma once

#include <format>
#include <iostream>
#include <string>

#include "periph/pwm.hpp"

namespace mcal::raspi {

class PWMOutput : public macfe::periph::PWMOutput {
public:
    PWMOutput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << std::format("Starting PWM {}", name_) << std::endl;
    }

    void Stop() override {
        std::cout << std::format("Stopping PWM {}", name_) << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        duty_cycle_ = macfe::util::Clamper<float>::Evaluate(duty_cycle, 0, 100);

        std::cout << std::format("Setting PWM {} to {:.3g}%", name_,
                                 duty_cycle_)
                  << std::endl;
    }
    float GetDutyCycle() override {
        std::cout << std::format("PWM {} has duty cycle {:.3f} Hz", name_,
                                 duty_cycle_)
                  << std::endl;
        return duty_cycle_;
    }

private:
    std::string name_;
    float duty_cycle_;
};

}  // namespace mcal::raspi
