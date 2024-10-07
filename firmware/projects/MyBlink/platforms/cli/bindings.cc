#include <unistd.h>

#include <iostream>

#include "../../bindings.h"
#include "mcal/cli/periph/pwm.h"
#include "shared/periph/pwm.h"
#include "mcal/cli/periph/analog_output.h"
#include "shared/periph/analog_output.h"

namespace mcal {

cli::periph::PWMOutput test{"Test"};
cli::periph::AnalogOutput test2{"Test 2"};

}  // namespace mcal

namespace bindings {

shared::periph::PWMOutput& test = mcal::test;
shared::periph::AnalogOutput& test2 = mcal::test2;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings
