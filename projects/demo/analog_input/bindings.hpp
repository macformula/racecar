#pragma once

#include "lib/periph/analog_input.hpp"
#include "lib/periph/gpio.hpp"

namespace bindings {

extern macfe::periph::AnalogInput& analog_input;

extern macfe::periph::DigitalOutput& red_led;
extern macfe::periph::DigitalOutput& blue_led;
extern macfe::periph::DigitalOutput& green_led;

extern void Init();

}  // namespace bindings