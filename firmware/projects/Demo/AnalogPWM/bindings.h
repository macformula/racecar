#pragma once

#include "shared/periph/pwm.h"
#include "shared/periph/analog_output.h"

namespace bindings {

extern shared::periph::PWMOutput& pwm;
extern shared::periph::AnalogOutput& analog_out;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings