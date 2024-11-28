#pragma once

#include "shared/periph/gpio.h"

namespace bindings {

extern shared::periph::DigitalInput& button_di;
extern shared::periph::DigitalOutput& indicator_do;

extern void Initialize();

}  // namespace bindings