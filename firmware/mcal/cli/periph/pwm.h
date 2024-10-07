/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <iostream>
#include <string>

#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"

namespace mcal::cli::periph {

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

    void SetFrequency(float frequency) override {
        frequency_ = frequency;

        std::cout << "Setting frequency " << name_ << " to " << frequency_
                  << " Hz" << std::endl;
    }

    float GetFrequency() override {
        std::cout << "Frequency " << name_ << " has frequency " << frequency_
                  << " Hz" << std::endl;
        return frequency_;
    }

private:
    std::string name_;
    float duty_cycle_;
    float frequency_;
};

}  // namespace mcal::cli::periph
