#include "bindings.hpp"

#include <iostream>

#include "mcal/linux/analog_input.hpp"
#include "mcal/linux/digital_output.hpp"
#include "periph/analog_input.hpp"
#include "periph/gpio.hpp"

namespace mcal {

lnx::AnalogInput analog_input{"analog input"};
lnx::DigitalOutput green_led{"green_led"};
lnx::DigitalOutput red_led{"red_led"};
lnx::DigitalOutput blue_led{"blue_led"};
}  // namespace mcal

namespace bindings {

macfe::periph::AnalogInput& analog_input = mcal::analog_input;
macfe::periph::DigitalOutput& green_led = mcal::green_led;
macfe::periph::DigitalOutput& red_led = mcal::red_led;
macfe::periph::DigitalOutput& blue_led = mcal::blue_led;

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings