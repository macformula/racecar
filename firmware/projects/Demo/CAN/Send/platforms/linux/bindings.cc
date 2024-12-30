#include <unistd.h>

#include <iostream>

#include "../../bindings.hpp"
#include "mcal/linux/periph/can.hpp"
#include "mcal/linux/periph/digital_input.hpp"

namespace mcal {
using namespace lnx::periph;

CanBase demo_can_base{"vcan0"};
DigitalInput button{"Button"};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& demo_can_base = mcal::demo_can_base;
shared::periph::DigitalInput& button = mcal::button;

void TickBlocking(uint32_t ms) {
    usleep(1000 * ms);
}

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::demo_can_base.Setup();
}

}  // namespace bindings