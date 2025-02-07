#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalInput& button;

extern shared::periph::AnalogInput& analog_input;
extern shared::periph::AnalogOutput& analog_output;

extern shared::periph::DigitalOutput& red_led;
extern shared::periph::DigitalOutput& blue_led;
extern shared::periph::DigitalOutput& green_led;

extern void DelayMs(uint32_t ms);
extern void Init();

}  // namespace bindings