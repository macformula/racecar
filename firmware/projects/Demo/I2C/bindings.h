#pragma once

#include "shared/periph/gpio.hpp"
#include "shared/periph/i2c.hpp"

namespace bindings {

extern shared::periph::DigitalOutput& indicator;
extern shared::periph::I2C& i2c_bus1;
extern shared::periph::I2C& i2c_bus2;

extern void DelayMS(unsigned int ms);
extern void Initialize();

}  // namespace bindings