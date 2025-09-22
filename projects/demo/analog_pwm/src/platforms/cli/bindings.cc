#include "bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "mcal/cli/analog_output.hpp"
#include "mcal/cli/pwm.hpp"
#include "periph/analog_output.hpp"
#include "periph/pwm.hpp"

namespace mcal {

cli::PWMOutput pwm{"test"};
cli::AnalogOutput analog_out{"test"};

}  // namespace mcal

namespace bindings {

macfe::periph::PWMOutput& pwm = mcal::pwm;
macfe::periph::AnalogOutput& analog_out = mcal::analog_out;

void DelayMS(unsigned int ms) {
    usleep(ms * 1000);
}

void Initialize() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings
