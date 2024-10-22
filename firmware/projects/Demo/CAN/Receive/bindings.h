#pragma once

#include "shared/periph/can.h"
#include "shared/periph/gpio.h"

namespace bindings {

extern shared::periph::CanBase& veh_can_base;
extern shared::periph::DigitalOutput& indicator;

extern void Initialize();
}  // namespace bindings