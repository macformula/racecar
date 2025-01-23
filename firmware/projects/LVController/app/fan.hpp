#pragma once

#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "subsystem.hpp"


class Fan : public Subsystem {
public:
    Fan(shared::periph::DigitalOutput& enable_output,
        shared::periph::PWMOutput& pwm_output,
        shared::util::Mapper<float>& power_to_duty);

    void Enable() override;
    void Disable() override;
    void SetTargetPower(float power, float max_duty_per_second);
    bool IsAtTarget();

    /// @brief Set the Power Now.
    /// @warning THIS IS DANGEROUS as large jumps can caused significant current
    /// rushes. Prefer SetTargetPower() and Update()
    void Dangerous_SetPowerNow(float power);

    /// @brief Update the duty cycle towards the target duty cycle. The argument
    /// is the elapsed time between calls which is required to adjust the duty
    /// cycle by the rate specified in SetTargetPower.
    void Update(float interval_sec);

private:
    shared::periph::PWMOutput& pwm_output_;
    shared::util::Mapper<float>& power_to_duty_;
    float target_duty_ = 0.0f;
    float duty_per_second_ = 0.0f;

    void SetPower(float power) const {
        pwm_output_.SetDutyCycle(power_to_duty_.Evaluate(power));
    }
};
