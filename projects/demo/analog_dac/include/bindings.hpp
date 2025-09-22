#pragma once

#include "periph/analog_output.hpp"

namespace bindings {

extern macfe::periph::AnalogOutput& analog_out_dac;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings