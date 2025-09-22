#pragma once

#include "periph/can.hpp"
#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::CanBase& demo_can_base;
extern macfe::periph::DigitalOutput& indicator;

extern void Initialize();
}  // namespace bindings