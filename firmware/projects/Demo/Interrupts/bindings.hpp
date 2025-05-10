#pragma once

#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& indicator;
extern void Initialize();

}  // namespace bindings

// Declaring the handler in bindings allows the platforms to call it.
extern void AppInterruptHandler(int);