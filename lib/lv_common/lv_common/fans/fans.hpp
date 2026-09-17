#pragma once

#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace fans {

struct fans_periph {
    macfe::periph::PWMOutput* powertrain_fan_pwm;
    macfe::periph::DigitalOutput* powertrain_fan1_en;
    macfe::periph::DigitalOutput* powertrain_fan2_en;
};

class Controller {
public:
    //! Since these periphs are accessible to everyone, make all controllers
    //! take periph as input
    Controller(fans_periph fans_periph) : periph(fans_periph) {};

    void SetPowerSetpoint(float power);
    bool IsAtSetpoint(void);

    void Update_100Hz(void);

private:
    void UpdateEnabled();
    void UpdateStep();
    fans_periph periph;
    float power_setpoint = 0;
    float current_power = 0;
};

}  // namespace fans
