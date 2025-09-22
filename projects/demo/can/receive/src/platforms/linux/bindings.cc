#include "bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "mcal/linux/can.hpp"
#include "mcal/linux/digital_output.hpp"

namespace mcal {
using namespace lnx;

CanBase demo_can_base{"vcan0"};
DigitalOutput indicator{"Indicator"};

}  // namespace mcal

namespace bindings {

macfe::periph::CanBase& demo_can_base = mcal::demo_can_base;
macfe::periph::DigitalOutput& indicator = mcal::indicator;

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::demo_can_base.Setup();
}

}  // namespace bindings