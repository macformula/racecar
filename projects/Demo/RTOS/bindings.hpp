#pragma once

#include "lib/periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalInput& button;
extern macfe::periph::DigitalOutput& blue_led;
extern macfe::periph::DigitalOutput& red_led;

extern void Initialize();

}  // namespace bindings
