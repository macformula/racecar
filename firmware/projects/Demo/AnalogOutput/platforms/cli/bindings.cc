#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/analog_output.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {
using namespace mcal::cli::periph;

DigitalInput button{"button"};
AnalogOutput analog_output{"analog output"};
AnalogInputConnectedToAnalogOutput analog_input{"analog input", analog_output};
DigitalOutput green_led{"green_led"};
DigitalOutput red_led{"red_led"};
DigitalOutput blue_led{"blue_led"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalInput& button = mcal::button;
shared::periph::AnalogInput& analog_input = mcal::analog_input;
shared::periph::AnalogOutput& analog_output = mcal::analog_output;
shared::periph::DigitalOutput& green_led = mcal::green_led;
shared::periph::DigitalOutput& red_led = mcal::red_led;
shared::periph::DigitalOutput& blue_led = mcal::blue_led;

void DelayMs(uint32_t ms) {
    usleep(ms * 1000);
}

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings