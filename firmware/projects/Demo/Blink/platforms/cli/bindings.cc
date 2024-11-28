#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/periph/gpio.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {

cli::periph::DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings