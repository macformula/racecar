#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::AnalogInput& analog_input;

extern shared::periph::DigitalOutput& red_Led;
extern shared::periph::DigitalOutput& blue_Led;
extern shared::periph::DigitalOutput& green_Led;

extern void Init();

}  // namespace bindings