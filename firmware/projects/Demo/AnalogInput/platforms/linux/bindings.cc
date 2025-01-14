#include <iostream>

#include "../../bindings.hpp"
#include "mcal/linux/periph/analog_input.hpp"
#include "shared/periph/analog_input.hpp"

namespace mcal {

lnx::periph::AnalogInput analog_input{"analog input"};

}  // namespace mcal

namespace bindings {

shared::periph::AnalogInput& analog_input = mcal::analog_input;

void Init() {
    std::cout << "Initializing the CLI..." << std::endl;
}

}  // namespace bindings