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

void Fan::SetPower(float power) {
    pwm_output_.SetDutyCycle(power_to_duty_.Evaluate(power));
}

void Fan::Dangerous_SetPowerNow(float power) {
    SetPower(power);
}

float Fan::Sweep::Interpolate(float time_ms) {
    float lerp_fraction = shared::util::Clamper<float>::Evaluate(
        (time_ms - start_time_ms) / duration_ms, 0.f, 1.f);
    return start_power + lerp_fraction * (end_power - start_power);
}

void Fan::StartSweep(Sweep sweep_config) {
    sweep_ = sweep_config;
    sweep_complete_ = false;
    Dangerous_SetPowerNow(sweep_.start_power);
}

void Fan::UpdateSweep(float time_ms) {
    float new_power = sweep_.Interpolate(time_ms);
    if (new_power >= sweep_.end_power) {
        sweep_complete_ = true;
    } else {
        SetPower(new_power);
    }
}

bool Fan::IsSweepComplete() {
    return sweep_complete_;
}