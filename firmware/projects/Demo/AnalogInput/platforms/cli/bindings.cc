#include <iostream>

#include "../../bindings.h"
#include "mcal/cli/periph/analog_input.h"
#include "shared/periph/analog_input.h"

namespace mcal {

cli::periph::AnalogInput analog_input{"analog input"};

}  // namespace mcal

namespace bindings {

shared::periph::AnalogInput& analog_input = mcal::analog_input;

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings