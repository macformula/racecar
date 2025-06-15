#pragma once

#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalInput& button;
extern shared::periph::DigitalOutput& blue_led;
extern shared::periph::DigitalOutput& red_led;

extern void Initialize();

}  // namespace bindings
