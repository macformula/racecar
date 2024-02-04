/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_PWM_H_
#define MCAL_WINDOWS_PERIPH_PWM_H_

#include <iostream>

#include "shared/periph/pwm.h"

namespace mcal::periph {

class PWMOutput : public shared::periph::PWMOutput {
public:
    PWMOutput(int channel) : channel_(channel) {}

    void Start() override {
        std::cout << "Starting PWM " << channel_ << std::endl;
    }

    void Stop() override {
        std::cout << "Stopping PWM " << channel_ << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        // clamp duty cycle between 0, 100
        duty_cycle_ = (duty_cycle < 0.0f)     ? 0.0f
                      : (duty_cycle > 100.0f) ? 100.0f
                                              : duty_cycle;

        std::cout << "Setting PWM " << channel_ << " to " << duty_cycle_ << "%"
                  << std::endl;
    }
    float GetDutyCycle() override {
        std::cout << "PWM " << channel_ << " has duty cycle " << duty_cycle_
                  << "%" << std::endl;
        return duty_cycle_;
    }

private:
    int channel_;
    float duty_cycle_;
};

}  // namespace mcal::periph

#endif