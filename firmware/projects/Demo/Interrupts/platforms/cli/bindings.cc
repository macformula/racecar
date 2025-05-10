#include "../../bindings.hpp"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "mcal/cli/periph/gpio.h"
#include "shared/periph/gpio.h"

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
    std::cout << "Press ctrl+c to simulate the interrupt" << std::endl;
    std::cout << "Press ctrl+\\ to exit.\n" << std::endl;

    // register the signal handler for SIGINT (ctrl+c)
    // this is used to simulate the interrupt on the CLI
    signal(SIGINT, AppInterruptHandler);
}

}  // namespace bindings
