/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_PWM_H_
#define MCAL_WINDOWS_PERIPH_PWM_H_

#include <iostream>
#include <string>

#include "shared/util/peripheral.h"

namespace mcal::periph {

class PWMOutput : public shared::util::Peripheral {
public:
    PWMOutput(std::string name) : name_(name) {}

    void Start() {
        std::cout << "Starting PWM " << name_ << std::endl;
    }

    void Stop() {
        std::cout << "Stopping PWM " << name_ << std::endl;
    }

    void SetDutyCycle(float duty_cycle) {
        // clamp duty cycle between 0, 100
        duty_cycle_ = (duty_cycle < 0.0f)     ? 0.0f
                      : (duty_cycle > 100.0f) ? 100.0f
                                              : duty_cycle;

        std::cout << "Setting PWM " << name_ << " to " << duty_cycle_ << "%"
                  << std::endl;
    }
    float GetDutyCycle() {
        std::cout << "PWM " << name_ << " has duty cycle " << duty_cycle_ << "%"
                  << std::endl;
        return duty_cycle_;
    }

private:
    std::string name_;
    float duty_cycle_;
};

}  // namespace mcal::periph

#endif