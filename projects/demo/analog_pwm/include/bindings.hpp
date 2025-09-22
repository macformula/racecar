#pragma once

#include "periph/analog_output.hpp"
#include "periph/pwm.hpp"

namespace bindings {

extern macfe::periph::PWMOutput& pwm;
extern macfe::periph::AnalogOutput& analog_out;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings