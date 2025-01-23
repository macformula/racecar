#include "fan.hpp"

#include <cmath>

#include "shared/util/mappers/clamper.hpp"

Fan::Fan(shared::periph::DigitalOutput& enable_output,
         shared::periph::PWMOutput& pwm_output,
         shared::util::Mapper<float>& power_to_duty)
    : Subsystem(enable_output),
      pwm_output_(pwm_output),
      power_to_duty_(power_to_duty) {}

void Fan::Enable() {
    SetPower(0.0f);
    Subsystem::Enable();
}

void Fan::Disable() {
    SetPower(0.0f);
    Subsystem::Disable();
}

void Fan::SetTargetPower(float power, float max_duty_per_second) {
    target_duty_ = power_to_duty_.Evaluate(power);
    duty_per_second_ = max_duty_per_second;
}

bool Fan::IsAtTarget() {
    constexpr float kDutyEqualTolerance = 0.1f;
    float error = std::abs(pwm_output_.GetDutyCycle() - target_duty_);
    return error < kDutyEqualTolerance;
}

void Fan::Dangerous_SetPowerNow(float power) {
    SetPower(power);
}

void Fan::Update(float interval_sec) {
    // max_duty_step must be greater than any platform's PWM duty cycle
    // resolution
    float max_duty_step = duty_per_second_ * interval_sec;

    float current_duty = pwm_output_.GetDutyCycle();
    float duty_step = shared::util::Clamper<float>::Evaluate(
        target_duty_ - current_duty, -max_duty_step, +max_duty_step);

    pwm_output_.SetDutyCycle(current_duty + duty_step);
}
