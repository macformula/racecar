#pragma once

#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalOutput& indicator;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings