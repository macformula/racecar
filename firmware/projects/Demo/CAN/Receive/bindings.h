#pragma once

#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::CanBase& veh_can_base;
extern shared::periph::DigitalOutput& indicator;

extern void Initialize();
}  // namespace bindings