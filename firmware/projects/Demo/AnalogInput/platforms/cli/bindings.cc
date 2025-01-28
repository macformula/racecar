#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/analog_input.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {

cli::periph::AnalogInput analog_input{"analog input"};
cli::periph::DigitalOutput green_led{"green_led"};
cli::periph::DigitalOutput red_led{"red_led"};
cli::periph::DigitalOutput blue_led{"blue_led"};

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