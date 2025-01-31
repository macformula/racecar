#include "fan.hpp"

#include <cmath>

#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/clamper.hpp"

Fans::Fans(shared::periph::DigitalOutput& enable_output1,
           shared::periph::DigitalOutput& enable_output2,
           shared::periph::PWMOutput& pwm_output,
           shared::util::Mapper<float>& power_to_duty)
    : enable_output1_(enable_output1),
      enable_output2_(enable_output2),
      pwm_output_(pwm_output),
      power_to_duty_(power_to_duty) {}

void Fans::Enable() {
    enable_output1_.SetHigh();
    enable_output2_.SetHigh();
    pwm_output_.Start();
    SetPower(0.0f);
}

void Fans::Disable() {
    enable_output1_.SetLow();
    enable_output2_.SetLow();
    pwm_output_.Stop();
    SetPower(0.0f);
}

void Fans::SetPower(float power) {
    pwm_output_.SetDutyCycle(power_to_duty_.Evaluate(power));
}

void Fans::Dangerous_SetPowerNow(float power) {
    SetPower(power);
}

float Fans::Sweep::Interpolate(float time_ms) {
    float lerp_fraction = shared::util::Clamper<float>::Evaluate(
        (time_ms - start_time_ms) / duration_ms, 0.f, 1.f);
    return start_power + lerp_fraction * (end_power - start_power);
}

void Fans::StartSweep(Sweep sweep_config) {
    sweep_ = sweep_config;
    sweep_complete_ = false;
    Dangerous_SetPowerNow(sweep_.start_power);
}

void Fans::UpdateSweep(float time_ms) {
    float new_power = sweep_.Interpolate(time_ms);
    if (new_power >= sweep_.end_power) {
        sweep_complete_ = true;
    } else {
        SetPower(new_power);
    }
}

bool Fans::IsSweepComplete() {
    return sweep_complete_;
}