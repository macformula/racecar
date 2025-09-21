#pragma once

#include "lib/periph/analog_output.hpp"
#include "lib/periph/pwm.hpp"

namespace bindings {

extern macfe::periph::PWMOutput& pwm;
extern macfe::periph::AnalogOutput& analog_out;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings