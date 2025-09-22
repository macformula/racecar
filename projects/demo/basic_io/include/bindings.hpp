#pragma once

#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalInput& button_di;
extern macfe::periph::DigitalOutput& indicator_do;

extern void Initialize();

}  // namespace bindings