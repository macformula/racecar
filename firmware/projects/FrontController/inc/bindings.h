/// @author Blake Freer
/// @date 2024-05-29

#pragma once

#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"

namespace bindings {

extern shared::periph::DigitalOutput&& driver_speaker;
extern shared::periph::DigitalOutput&& brake_light;
extern shared::periph::DigitalOutput&& status_light;
extern shared::periph::ADCInput&& accel_pedal_1;
extern shared::periph::ADCInput&& accel_pedal_2;
extern shared::periph::ADCInput&& front_brake_pedal;
extern shared::periph::ADCInput&& steering_wheel;
extern shared::periph::DigitalInput&& start_button;
extern shared::periph::CanBase&& motor_left_can;
extern shared::periph::CanBase&& motor_right_can;
extern shared::periph::CanBase&& contactor_can;

extern void Initialize();
}  // namespace bindings
