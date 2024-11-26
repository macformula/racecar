#include <unistd.h>

#include <iostream>

#include "../../bindings.h"
#include "mcal/linux/periph/can.hpp"
#include "mcal/linux/periph/digital_input.hpp"

namespace mcal {
using namespace lnx::periph;

CanBase veh_can_base{"vcan0"};
DigitalInput button{"Button"};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalInput& button = mcal::button;

void TickBlocking(uint32_t ms) {
    usleep(1000 * ms);
}

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::veh_can_base.Setup();
}

}  // namespace bindings