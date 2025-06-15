#include "../../bindings.hpp"

#include <signal.h>
#include <stdlib.h>

#include <iostream>

#include "mcal/cli/gpio.hpp"
#include "shared/periph/gpio.hpp"

namespace mcal {

cli::DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

shared::periph::DigitalOutput& indicator = mcal::indicator;

static void InterruptHandler(int) {
    AppInterruptHandler(0);

    // We must re-register the SIGINT handler each time. If this wasn't the
    // case, then this enclosing function would be unnecessary since the
    // signal() call in Initialize() could directly register AppInterruptHandler
    signal(SIGINT, InterruptHandler);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
    std::cout << "Press ctrl+c to simulate the interrupt" << std::endl;

#ifdef __unix__
    std::cout << "Press ctrl+\\ to exit.\n" << std::endl;
#else
    std::cout << "To end the program: " << std::endl;
    std::cout << "  Command Prompt: ctrl+fn+b" << std::endl;
    std::cout << "  Git Bash: Close the terminal session\n" << std::endl;
#endif

    // register the signal handler for SIGINT (ctrl+c)
    // this is used to simulate the interrupt on the CLI
    signal(SIGINT, InterruptHandler);
}

}  // namespace bindings
