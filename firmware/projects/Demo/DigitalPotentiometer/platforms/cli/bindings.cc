#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/analog_output.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/i2c.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/i2c.hpp"

namespace mcal {
using namespace mcal::cli::periph;

DigitalInput button_increment_pot0{"button_increment_pot0"};
DigitalInput button_increment_pot1{"button_increment_pot1"};
DigitalInput button_terminal_connection_pot0{"button_terminal_connection_pot0"};
DigitalInput button_terminal_connection_pot1{"button_terminal_connection_pot1"};

I2CBus i2c_bus1{1};

DigitalOutput green_led{"green_led"};
DigitalOutput red_led{"red_led"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalInput& button_increment_pot0 =
    mcal::button_increment_pot0;
shared::periph::DigitalInput& button_increment_pot1 =
    mcal::button_increment_pot1;
shared::periph::DigitalInput& button_terminal_connection_pot0 =
    mcal::button_terminal_connection_pot0;
shared::periph::DigitalInput& button_terminal_connection_pot1 =
    mcal::button_terminal_connection_pot1;

shared::periph::DigitalOutput& green_led = mcal::green_led;
shared::periph::DigitalOutput& red_led = mcal::red_led;

shared::periph::I2CBus& i2c_bus1 = mcal::i2c_bus1;

void DelayMs(uint32_t ms) {
    usleep(ms * 1000);
}

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings