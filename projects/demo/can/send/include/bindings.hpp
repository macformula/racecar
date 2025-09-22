#pragma once

#include <cstdint>

#include "periph/can.hpp"
#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::CanBase& demo_can_base;
extern macfe::periph::DigitalInput& button;

extern void Initialize();
extern void TickBlocking(uint32_t);

}  // namespace bindings