#pragma once

#include "shared/comms/can/can_bus.hpp"

namespace bindings {
extern shared::periph::CanBase& error_can_base;
extern void Initialize();
extern void TickBlocking(uint32_t);
}  // namespace bindings