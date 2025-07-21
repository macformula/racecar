#pragma once

#include "shared/periph/analog_output.hpp"
#include "shared/periph/pwm.hpp"

namespace bindings {

extern shared::periph::PWMOutput& pwm;
extern shared::periph::AnalogOutput& analog_out;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings