/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <format>
#include <iostream>
#include <string>

#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"

namespace mcal::cli::periph {

class PWMOutput : public shared::periph::PWMOutput {
public:
    PWMOutput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << std::format("Starting PWM {}", name_) << std::endl;
    }

    void Stop() override {
        std::cout << std::format("Stopping PWM {}", name_) << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        duty_cycle_ =
            shared::util::Clamper<float>::Evaluate(duty_cycle, 0, 100);

        std::cout << std::format("Setting PWM {} duty cycle to {:.3g}%", name_,
                                 duty_cycle_)
                  << std::endl;
    }

    float GetDutyCycle() override {
        std::cout << std::format("PWM {} has duty cycle {:.3g} %", name_,
                                 duty_cycle_)
                  << std::endl;
        return duty_cycle_;
    }

    void SetFrequency(float frequency) override {
        frequency_ = std::max((float)0, frequency);

        std::cout << std::format("Setting PWM {} frequency to {:.3f} Hz", name_,
                                 frequency_)
                  << std::endl;
    }

    float GetFrequency() override {
        std::cout << std::format("PWM {} has frequency {:.3f} %", name_,
                                 frequency_)
                  << std::endl;
        return frequency_;
    }

private:
    std::string name_;
    float duty_cycle_;
    float frequency_;
};

}  // namespace mcal::cli::periph
