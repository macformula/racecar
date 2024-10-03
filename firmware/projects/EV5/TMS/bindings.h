#pragma once

#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"

namespace bindings {
extern shared::periph::ADCInput& temp_sensor_adc_1;
extern shared::periph::ADCInput& temp_sensor_adc_2;
extern shared::periph::ADCInput& temp_sensor_adc_3;
extern shared::periph::ADCInput& temp_sensor_adc_4;
extern shared::periph::ADCInput& temp_sensor_adc_5;
extern shared::periph::ADCInput& temp_sensor_adc_6;

extern shared::periph::PWMOutput& fan_controller_pwm;

extern shared::periph::DigitalOutput& debug_led_green;
extern shared::periph::DigitalOutput& debug_led_red;

extern shared::periph::CanBase& veh_can_base;

extern void Initialize();
}  // namespace bindings
