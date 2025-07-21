#include "fans.hpp"

#include <cmath>

#include "etl/algorithm.h"
#include "inc/bindings.hpp"
#include "shared/periph/gpio.hpp"

namespace fans {

static float power_setpoint = 0;
static float current_power = 0;

void SetPowerSetpoint(float power) {
    power_setpoint = etl::clamp<float>(power, 0, 100);
}

bool IsAtSetpoint(void) {
    static const float kThresholdEqual = 1;
    return std::abs(power_setpoint - current_power) < kThresholdEqual;
}

void Init(void) {
    bindings::powertrain_fan_pwm.Start();

    power_setpoint = 0;
    current_power = 0;
    bindings::powertrain_fan_pwm.SetDutyCycle(power_setpoint);
}

static void UpdateEnabled(void) {
    static const float kEnableThreshold = 5;

    // disable until the below TODO is addressed (to avoid blowing a fuse)
    bool enabled = false;
    // bool enabled = current_power > kEnableThreshold;

    bindings::powertrain_fan1_en.Set(enabled);
    bindings::powertrain_fan2_en.Set(enabled);
}

static void UpdateStep(void) {
    static const float kMaxFanPowerPerMs = 20e-3;  // related to task period
    static const float kTaskMs = 10;

    float step =
        etl::clamp(power_setpoint - current_power, -kMaxFanPowerPerMs * kTaskMs,
                   kMaxFanPowerPerMs * kTaskMs);

    current_power += step;

    // TODO: should the duty be inverted?
    bindings::powertrain_fan_pwm.SetDutyCycle(current_power);
}

void Update_100Hz(void) {
    UpdateEnabled();
    UpdateStep();
}

}  // namespace fans
