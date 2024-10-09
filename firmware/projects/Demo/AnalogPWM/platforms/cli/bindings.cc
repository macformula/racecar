#include <unistd.h>

#include <iostream>

#include "../../bindings.h"
#include "mcal/cli/periph/pwm.h"
#include "shared/periph/pwm.h"
#include "mcal/cli/periph/analog_output.h"
#include "shared/periph/analog_output.h"

namespace mcal {

cli::periph::PWMOutput pwm{"test"};
cli::periph::AnalogOutput analog_out{"test"};

}  // namespace mcal

namespace bindings {

shared::periph::PWMOutput& pwm = mcal::pwm;
shared::periph::AnalogOutput& analog_out = mcal::analog_out;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings
