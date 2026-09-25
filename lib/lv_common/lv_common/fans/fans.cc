#include "fans.hpp"

#include <cmath>

#include "etl/algorithm.h"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace macfe::lv {

void Fans::SetPowerSetpoint(float power) {
    power_setpoint = etl::clamp<float>(power, 0, 100);
}

bool Fans::IsAtSetpoint(void) {
    static const float kThresholdEqual = 1;
    return std::abs(power_setpoint - current_power) < kThresholdEqual;
}

void Fans::UpdateEnabled() {
    // static const float kEnableThreshold = 5;

    // disable until the below TODO is addressed (to avoid blowing a fuse)
    bool enabled = false;
    // bool enabled = current_power > kEnableThreshold;

    _powertrain_fan1_en.Set(enabled);
    _powertrain_fan2_en.Set(enabled);
}

void Fans::UpdateStep() {
    static const float kMaxFanPowerPerMs = 20e-3;  // related to task period
    static const float kTaskMs = 10;

    float step =
        etl::clamp(power_setpoint - current_power, -kMaxFanPowerPerMs * kTaskMs,
                   kMaxFanPowerPerMs * kTaskMs);

    current_power += step;

    // TODO: should the duty be inverted?
    _powertrain_fan_pwm.SetDutyCycle(current_power);
}

void Fans::Update_100Hz() {
    UpdateEnabled();
    UpdateStep();
}

}  // namespace macfe::lv