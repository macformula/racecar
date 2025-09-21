#pragma once

#include <cstdint>

#include "lib/periph/can.hpp"

namespace bindings {
extern macfe::periph::CanBase& error_can_base;
extern void Initialize();
extern void TickBlocking(uint32_t);
}  // namespace bindings
