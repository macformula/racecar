#include "../../bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/cli/analog_output.hpp"
#include "mcal/cli/pwm.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/pwm.hpp"

namespace mcal {

cli::PWMOutput pwm{"test"};
cli::AnalogOutput analog_out{"test"};

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
