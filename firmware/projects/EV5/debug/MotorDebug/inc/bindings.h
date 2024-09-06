#pragma once

#include <cstdint>

#include "shared/periph/adc.h"

namespace bindings {

extern shared::periph::ADCInput&& accel_pedal_1;
extern shared::periph::ADCInput&& accel_pedal_2;

extern void Initialize();

extern void DelayMS(uint32_t ms);

} // namespace bindings