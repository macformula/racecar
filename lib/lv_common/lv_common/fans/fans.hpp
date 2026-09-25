#pragma once

#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace macfe::lv {

macfe::periph::PWMOutput& powertrain_fan_pwm;
macfe::periph::DigitalOutput& powertrain_fan1_en;
macfe::periph::DigitalOutput& powertrain_fan2_en;

class Fans {
public:
    Fans(macfe::periph::PWMOutput& powertrain_fan_pwm,
         macfe::periph::DigitalOutput& powertrain_fan1_en,
         macfe::periph::DigitalOutput& powertrain_fan2_en)
        : _powertrain_fan_pwm(powertrain_fan_pwm),
          _powertrain_fan1_en(powertrain_fan1_en),
          _powertrain_fan2_en(powertrain_fan2_en) {
        _powertrain_fan_pwm.Start();

        power_setpoint = 0;
        current_power = 0;
        _powertrain_fan_pwm.SetDutyCycle(power_setpoint);
    };

    void SetPowerSetpoint(float power);
    bool IsAtSetpoint(void);

    void Update_100Hz(void);

private:
    void UpdateEnabled();
    void UpdateStep();
    macfe::periph::PWMOutput& _powertrain_fan_pwm;
    macfe::periph::DigitalOutput& _powertrain_fan1_en;
    macfe::periph::DigitalOutput& _powertrain_fan2_en;
    float power_setpoint = 0;
    float current_power = 0;
};

}  // namespace macfe::lv
