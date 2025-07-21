#pragma once

#include "periph/analog_input.hpp"
#include "periph/can.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

namespace bindings {
extern macfe::periph::AnalogInput& temp_sensor_adc_1;
extern macfe::periph::AnalogInput& temp_sensor_adc_2;
extern macfe::periph::AnalogInput& temp_sensor_adc_3;
extern macfe::periph::AnalogInput& temp_sensor_adc_4;
extern macfe::periph::AnalogInput& temp_sensor_adc_5;
extern macfe::periph::AnalogInput& temp_sensor_adc_6;

extern macfe::periph::PWMOutput& fan_controller_pwm;

extern macfe::periph::DigitalOutput& debug_led_green;
extern macfe::periph::DigitalOutput& debug_led_red;

extern macfe::periph::CanBase& veh_can_base;

extern void Initialize();
extern uint32_t GetCurrentTimeMs();

extern void SoftwareReset();

}  // namespace bindings
