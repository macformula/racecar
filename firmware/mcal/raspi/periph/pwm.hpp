/// @author Samuel Parent
/// @date 2023-05-21

#pragma once

#include <iostream>
#include <string>

#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/clamper.hpp"

namespace mcal::raspi::periph {

class PWMOutput : public shared::periph::PWMOutput {
public:
    PWMOutput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << "Starting PWM " << name_ << std::endl;
    }

    void Stop() override {
        std::cout << "Stopping PWM " << name_ << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        duty_cycle_ =
            shared::util::Clamper<float>::Evaluate(duty_cycle, 0, 100);

        std::cout << "Setting PWM " << name_ << " to " << duty_cycle_ << "%"
                  << std::endl;
    }
    float GetDutyCycle() override {
        std::cout << "PWM " << name_ << " has duty cycle " << duty_cycle_ << "%"
                  << std::endl;
        return duty_cycle_;
    }

private:
    std::string name_;
    float duty_cycle_;
};

}  // namespace mcal::raspi::periph
