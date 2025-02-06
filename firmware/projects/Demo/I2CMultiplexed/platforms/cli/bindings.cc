#include <unistd.h>

#include <iostream>

#include "../../bindings.h"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/i2c.hpp"
#include "shared/device/ti/tca9548a.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/i2c.hpp"

namespace mcal {
using namespace cli::periph;

// Base I2C busses
I2CBus i2c_bus1(1);
DigitalInput button1("button1");

}  // namespace mcal

namespace device {
using namespace shared::device;

ti::TCA9548A tca9548a_i2c_mux1(mcal::i2c_bus1, 0x70);

I2CMultiplexer<ti::TCA9548AChannel>& i2c_mux1 = tca9548a_i2c_mux1;

I2CBusMultiplexed<ti::TCA9548AChannel> i2c_bus1A(
    i2c_mux1, ti::TCA9548AChannel::CHANNEL_A);
I2CBusMultiplexed<ti::TCA9548AChannel> i2c_bus1B(
    i2c_mux1, ti::TCA9548AChannel::CHANNEL_B);

}  // namespace device

namespace bindings {
shared::periph::I2CBus& i2c_bus1A = device::i2c_bus1A;
shared::periph::I2CBus& i2c_bus1B = device::i2c_bus1B;

shared::periph::DigitalInput& button1 = mcal::button1;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings