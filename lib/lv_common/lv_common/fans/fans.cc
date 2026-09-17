#include "fans.hpp"

#include <cmath>

#include "etl/algorithm.h"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace fans {

using macfe::periph::DigitalOutput;
using macfe::periph::PWMOutput;

void Controller::SetPowerSetpoint(float power) {
    power_setpoint = etl::clamp<float>(power, 0, 100);
}

bool Controller::IsAtSetpoint(void) {
    static const float kThresholdEqual = 1;
    return std::abs(power_setpoint - current_power) < kThresholdEqual;
}

void Controller::Controller(fans_periph fans_periph) {
    periph = fans_periph;
    periph.powertrain_fan_pwm->Start();

    power_setpoint = 0;
    current_power = 0;
    periph.powertrain_fan_pwm->SetDutyCycle(power_setpoint);
}

void Controller::UpdateEnabled() {
    // static const float kEnableThreshold = 5;

    // disable until the below TODO is addressed (to avoid blowing a fuse)
    bool enabled = false;
    // bool enabled = current_power > kEnableThreshold;

    periph.powertrain_fan1_en->Set(enabled);
    periph.powertrain_fan2_en->Set(enabled);
}

void Controller::UpdateStep() {
    static const float kMaxFanPowerPerMs = 20e-3;  // related to task period
    static const float kTaskMs = 10;

    float step =
        etl::clamp(power_setpoint - current_power, -kMaxFanPowerPerMs * kTaskMs,
                   kMaxFanPowerPerMs * kTaskMs);

    current_power += step;

    // TODO: should the duty be inverted?
    periph.powertrain_fan_pwm->SetDutyCycle(current_power);
}

void Controller::Update_100Hz() {
    UpdateEnabled();
    UpdateStep();
}

}  // namespace fans