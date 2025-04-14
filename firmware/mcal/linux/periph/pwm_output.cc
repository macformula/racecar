#include "pwm_output.hpp"

#include <format>
#include <iostream>
#include <string>

#include "etl/algorithm.h"

namespace mcal::lnx::periph {

PWMOutput::PWMOutput(std::string name) {}

void PWMOutput::Start() {
    std::cout << std::format("Starting PWM {}", name_) << std::endl;
}

void PWMOutput::Stop() {
    std::cout << std::format("Stopping PWM {}", name_) << std::endl;
}

void PWMOutput::SetDutyCycle(float duty_cycle) {
    duty_cycle = etl::clamp<float>(duty_cycle, 0, 100);

    std::cout << std::format("Setting PWM {} duty cycle to {:.3g}%", name_,
                             duty_cycle)
              << std::endl;
}

float PWMOutput::GetDutyCycle() {
    std::cout << std::format("PWM {} has duty cycle {:.3g}%", name_, duty_)
              << std::endl;
    return duty_;
}

void PWMOutput::SetFrequency(float frequency) {
    frequency_ = std::max((float)0, frequency);

    std::cout << std::format("Setting PWM {} frequency to {:.3f} Hz", name_,
                             frequency_)
              << std::endl;
}

float PWMOutput::GetFrequency() {
    std::cout << std::format("PWM {} has frequency {:.3f} Hz", name_,
                             frequency_)
              << std::endl;
    return frequency_;
}
}  // namespace mcal::lnx::periph