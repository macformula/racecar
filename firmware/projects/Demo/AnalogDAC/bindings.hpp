#pragma once

#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::AnalogOutput& analog_out_dac;
extern shared::periph::DigitalOutput& indicator;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings