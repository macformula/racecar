#pragma once

#include <cstdint>

#include "shared/periph/adc.h"
#include "shared/periph/can.h"

namespace bindings {

extern shared::periph::ADCInput& accel_pedal_1;
extern shared::periph::ADCInput& accel_pedal_2;
extern shared::periph::CanBase& vehicle_can_base;

extern void Initialize();

extern void DelayMS(uint32_t ms);

}  // namespace bindings