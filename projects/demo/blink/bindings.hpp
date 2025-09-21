#pragma once

#include "lib/periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalOutput& indicator;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings