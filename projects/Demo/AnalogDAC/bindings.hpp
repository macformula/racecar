#pragma once

#include "shared/periph/analog_output.hpp"

namespace bindings {

extern shared::periph::AnalogOutput& analog_out_dac;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings