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

//app level function
bool bindings::toggle = false;

//app level function that takes care of the interupt handler
void AppLevelFunction(){
    std::cout << "Toggle Switch is Pressed \n";
    bindings::toggle = !bindings::toggle; 
}

namespace bindings {

    shared::periph::DigitalOutput& indicator = mcal::indicator;

    void DelayMS(unsigned int ms) {
        usleep(ms * 1000);
    }

    void setup_interupt(){
        signal(SIGINT, (__p_sig_fn_t)&AppLevelFunction);
    }

    void Initialize() {
        std::cout << "Initializing the CLI...\n" << std::endl;
    }

}  // namespace bindings