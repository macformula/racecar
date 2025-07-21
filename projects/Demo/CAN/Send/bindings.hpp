#pragma once

#include <cstdint>

#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::CanBase& demo_can_base;
extern shared::periph::DigitalInput& button;

extern void Initialize();
extern void TickBlocking(uint32_t);

}  // namespace bindings