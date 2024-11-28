#pragma once

#include <cstdint>

#include "shared/periph/adc.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::ADCInput& accel_pedal_1;
extern shared::periph::ADCInput& accel_pedal_2;
extern shared::periph::CanBase& vehicle_can_base;
extern shared::periph::DigitalOutput& debug_led;

extern void Initialize();

extern void DelayMS(uint32_t ms);

}  // namespace bindings