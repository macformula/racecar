#pragma once

#include "shared/periph/pwm.h"
#include "shared/periph/analog_output.h"

namespace bindings {

extern shared::periph::PWMOutput& test;
extern shared::periph::AnalogOutput& test2;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings