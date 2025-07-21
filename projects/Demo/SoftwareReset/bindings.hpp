#pragma once

#include <stdint.h>

#include "shared/periph/gpio.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& red_led;
extern shared::periph::DigitalOutput& blue_led;
extern shared::periph::DigitalOutput& green_led;
extern shared::periph::DigitalInput& reset_button;

extern void Init();
extern void DelayMS(uint32_t ms);
extern void SoftwareReset();

}  // namespace bindings