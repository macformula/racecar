#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

namespace bindings {
extern shared::periph::AnalogInput& temp_sensor_adc_1;
extern shared::periph::AnalogInput& temp_sensor_adc_2;
extern shared::periph::AnalogInput& temp_sensor_adc_3;
extern shared::periph::AnalogInput& temp_sensor_adc_4;
extern shared::periph::AnalogInput& temp_sensor_adc_5;
extern shared::periph::AnalogInput& temp_sensor_adc_6;

extern shared::periph::PWMOutput& fan_controller_pwm;

extern shared::periph::DigitalOutput& debug_led_green;
extern shared::periph::DigitalOutput& debug_led_red;

extern shared::periph::CanBase& veh_can_base;

extern void Initialize();
extern uint32_t GetCurrentTimeMs();

extern void SoftwareReset();

}  // namespace bindings
