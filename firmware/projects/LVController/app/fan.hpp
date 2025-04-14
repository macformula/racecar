#pragma once

#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

class Fans {
public:
    struct Sweep {
        float start_time_ms;
        float duration_ms;
        float start_power;
        float end_power;

    private:
        friend class Fans;
        float Interpolate(float time_ms);
    };

    Fans(shared::periph::DigitalOutput& enable_output1,
         shared::periph::DigitalOutput& enable_output2,
         shared::periph::PWMOutput& pwm_output);

    void Enable();
    void Disable();
    bool IsAtTarget();

    /// @brief Set the Power Now.
    /// @warning THIS IS DANGEROUS as large jumps can caused significant current
    /// rushes. Prefer StartSweep() and Update()
    void Dangerous_SetPowerNow(float power);

    void StartSweep(Sweep sweep_config);
    void UpdateSweep(float time_ms);
    bool IsSweepComplete();

private:
    shared::periph::DigitalOutput& enable_output1_;
    shared::periph::DigitalOutput& enable_output2_;
    shared::periph::PWMOutput& pwm_output_;

    Sweep sweep_;
    bool sweep_complete_ = false;

    void SetPower(float power);
};
