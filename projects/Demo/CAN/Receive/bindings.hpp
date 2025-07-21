#pragma once

#include "lib/periph/can.hpp"
#include "lib/periph/gpio.hpp"

namespace bindings {

extern macfe::periph::CanBase& demo_can_base;
extern macfe::periph::DigitalOutput& indicator;

extern void Initialize();
}  // namespace bindings