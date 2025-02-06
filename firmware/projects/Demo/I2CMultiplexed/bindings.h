#pragma once

#include "shared/periph/gpio.hpp"
#include "shared/periph/i2c.hpp"

namespace bindings {

extern shared::periph::I2CBus& i2c_bus1A;
extern shared::periph::I2CBus& i2c_bus1B;
extern shared::periph::DigitalInput& button1;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings