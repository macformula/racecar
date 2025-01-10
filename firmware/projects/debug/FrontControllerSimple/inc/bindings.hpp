/// @author Blake Freer
/// @date 2024-05-29

#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& driver_speaker;
extern shared::periph::DigitalOutput& brake_light;
extern shared::periph::DigitalOutput& status_light;
extern shared::periph::AnalogInput& accel_pedal_1;
extern shared::periph::AnalogInput& accel_pedal_2;
extern shared::periph::AnalogInput& brake_pedal_front;
extern shared::periph::AnalogInput& brake_pedal_rear;
extern shared::periph::AnalogInput& steering_wheel;
extern shared::periph::DigitalInput& start_button;
extern shared::periph::CanBase& veh_can_base;
extern shared::periph::CanBase& pt_can_base;

extern void Initialize();
}  // namespace bindings
