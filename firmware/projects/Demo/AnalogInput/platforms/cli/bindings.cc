#include "../../bindings.hpp"

#include <iostream>

#include "mcal/cli/analog_input.hpp"
#include "mcal/cli/gpio.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {

cli::AnalogInput analog_input{"analog input"};
cli::DigitalOutput green_led{"green_led"};
cli::DigitalOutput red_led{"red_led"};
cli::DigitalOutput blue_led{"blue_led"};

}  // namespace mcal

namespace bindings {

shared::periph::AnalogInput& analog_input = mcal::analog_input;
shared::periph::DigitalOutput& green_led = mcal::green_led;
shared::periph::DigitalOutput& red_led = mcal::red_led;
shared::periph::DigitalOutput& blue_led = mcal::blue_led;

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings