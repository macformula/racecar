/// @author Blake Freer
/// @date 2023-12-10

#pragma once

#include <fmt/core.h>
#include <iostream>
#include <string>

#include "etl/algorithm.h"
#include "periph/pwm.hpp"

namespace mcal::cli {

class PWMOutput : public macfe::periph::PWMOutput {
public:
    PWMOutput(std::string name) : name_(name) {}

    void Start() override {
        std::cout << fmt::format("Starting PWM {}", name_) << std::endl;
    }

    void Stop() override {
        std::cout << fmt::format("Stopping PWM {}", name_) << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        duty_cycle_ = etl::clamp<float>(duty_cycle, 0, 100);

        std::cout << fmt::format("Setting PWM {} duty cycle to {:.3g}%", name_,
                                 duty_cycle_)
                  << std::endl;
    }

    float GetDutyCycle() override {
        std::cout << fmt::format("PWM {} has duty cycle {:.3g}%", name_,
                                 duty_cycle_)
                  << std::endl;
        return duty_cycle_;
    }

    void SetFrequency(float frequency) override {
        frequency_ = std::max((float)0, frequency);

        std::cout << fmt::format("Setting PWM {} frequency to {:.3f} Hz", name_,
                                 frequency_)
                  << std::endl;
    }

    float GetFrequency() override {
        std::cout << fmt::format("PWM {} has frequency {:.3f} Hz", name_,
                                 frequency_)
                  << std::endl;
        return frequency_;
    }

private:
    std::string name_;
    float duty_cycle_;
    float frequency_;
};

}  // namespace mcal::cli
