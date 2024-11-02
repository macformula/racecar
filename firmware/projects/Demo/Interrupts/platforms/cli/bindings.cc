#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include "../../bindings.h"
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

    // interupt function that uses the signal function for the cli/linux
    // platform

    void Initialize() {
        std::cout << "Initializing the CLI...\n" << std::endl;
        signal(SIGINT, (__p_sig_fn_t)&ToggleInterruptHandler);
    }

}  // namespace bindings