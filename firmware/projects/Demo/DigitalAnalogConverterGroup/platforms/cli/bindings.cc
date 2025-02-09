#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/analog_output.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "mcal/cli/periph/i2c.hpp"
#include "mcal/cli/periph/pwm.hpp"
#include "shared/device/dac.hpp"
#include "shared/device/mcp/mcp4728.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

namespace mcal {
using namespace mcal::cli::periph;

DigitalInput button_increment{"button_increment"};
DigitalInput button_decrement{"button_decrement"};
DigitalInput button_confirm{"button_confirm"};

DigitalOutput led_position0{"led_position0"};
DigitalOutput led_position1{"led_position1"};
DigitalOutput led_position2{"led_position2"};

PWMOutput pwm_output0{"pwm_output0"};

I2CBus i2c0{0};

}  // namespace mcal

namespace device {
using namespace shared::device;

mcp::MCP4728 mcp4728_dac0(mcal::i2c0, 0x60, bindings::kVoltageReference);
mcp::MCP4728 mcp4728_dac1(mcal::i2c0, 0x61, bindings::kVoltageReference);

DigitalAnalogConverter<bindings::kNumChannelsPerDac>& dac0 = mcp4728_dac0;
DigitalAnalogConverter<bindings::kNumChannelsPerDac>& dac1 = mcp4728_dac1;

DigitalAnalogConverter<bindings::kNumChannelsPerDac>* dacs[] = {&dac0, &dac1};

DigitalAnalogConverterGroup dac_group(dacs);

}  // namespace device

namespace bindings {

shared::periph::DigitalInput& button_increment = mcal::button_increment;
shared::periph::DigitalInput& button_decrement = mcal::button_decrement;
shared::periph::DigitalInput& button_confirm = mcal::button_confirm;

shared::periph::DigitalOutput& led_position0 = mcal::led_position0;
shared::periph::DigitalOutput& led_position1 = mcal::led_position1;
shared::periph::DigitalOutput& led_position2 = mcal::led_position2;
shared::periph::PWMOutput& pwm_output0 = mcal::pwm_output0;

shared::periph::AnalogOutputGroup<kNumAnalogOutputs>& analog_output_group =
    device::dac_group;

void DelayMs(uint32_t ms) {
    usleep(ms * 1000);
}

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings