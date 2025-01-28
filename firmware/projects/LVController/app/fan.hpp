#pragma once

#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "subsystem.hpp"

class Fan : public Subsystem {
public:
    struct Sweep {
        float start_time_ms;
        float duration_ms;
        float start_power;
        float end_power;

    private:
        friend class Fan;
        float Interpolate(float time_ms);
    };

    Fan(shared::periph::DigitalOutput& enable_output,
        shared::periph::PWMOutput& pwm_output,
        shared::util::Mapper<float>& power_to_duty);

    void Enable() override;
    void Disable() override;
    bool IsAtTarget();

    /// @brief Set the Power Now.
    /// @warning THIS IS DANGEROUS as large jumps can caused significant current
    /// rushes. Prefer SetTargetPower() and Update()
    void Dangerous_SetPowerNow(float power);

    void StartSweep(Sweep sweep_config);
    void UpdateSweep(float time_ms);
    bool IsSweepComplete();

private:
    shared::periph::PWMOutput& pwm_output_;
    shared::util::Mapper<float>& power_to_duty_;

    Sweep sweep_;
    bool sweep_complete_ = false;

    void SetPower(float power);
};
