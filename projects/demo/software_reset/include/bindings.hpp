#pragma once

#include <stdint.h>

#include "periph/gpio.hpp"

namespace bindings {

extern macfe::periph::DigitalOutput& red_led;
extern macfe::periph::DigitalOutput& blue_led;
extern macfe::periph::DigitalOutput& green_led;
extern macfe::periph::DigitalInput& reset_button;

extern void Init();
extern void DelayMS(uint32_t ms);
extern void SoftwareReset();

}  // namespace bindings