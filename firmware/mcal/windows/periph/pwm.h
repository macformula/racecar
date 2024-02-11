/// @author Blake Freer
/// @date 2023-12-10

#ifndef MCAL_WINDOWS_PERIPH_PWM_H_
#define MCAL_WINDOWS_PERIPH_PWM_H_

#include <iostream>
#include <string>

#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"

namespace mcal::periph {

class PWMOutput : public shared::periph::PWMOutput {
public:
    PWMOutput(std::string name) : name_(name), clamp_duty(0, 100) {}

    void Start() override {
        std::cout << "Starting PWM " << name_ << std::endl;
    }

    void Stop() override {
        std::cout << "Stopping PWM " << name_ << std::endl;
    }

    void SetDutyCycle(float duty_cycle) override {
        // clamp duty cycle between 0, 100
        duty_cycle_ = clamp_duty.Evaluate(duty_cycle);

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
    shared::util::Clamper clamp_duty;
};

}  // namespace mcal::periph

#endif