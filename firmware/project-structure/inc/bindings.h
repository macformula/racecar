#pragma once

#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& indicator;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings