#include "../../bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "mcal/linux/periph/can.hpp"
#include "mcal/linux/periph/digital_output.hpp"

namespace mcal {
using namespace lnx::periph;

CanBase demo_can_base{"vcan0"};
DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& demo_can_base = mcal::demo_can_base;
shared::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::demo_can_base.Setup();
}

}  // namespace bindings