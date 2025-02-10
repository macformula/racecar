#pragma once

#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/i2c.hpp"
namespace bindings {

extern shared::periph::DigitalInput& button_increment_pot0;
extern shared::periph::DigitalInput& button_increment_pot1;
extern shared::periph::DigitalInput& button_terminal_connection_pot0;
extern shared::periph::DigitalInput& button_terminal_connection_pot1;

extern shared::periph::I2CBus& i2c_bus1;

extern shared::periph::DigitalOutput& red_led;
extern shared::periph::DigitalOutput& green_led;

extern void DelayMs(uint32_t ms);
extern void Init();

}  // namespace bindings