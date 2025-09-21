#include "../../bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "lib/periph/gpio.hpp"
#include "mcal/cli/gpio.hpp"

namespace mcal {

cli::DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

macfe::periph::DigitalOutput& indicator = mcal::indicator;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings