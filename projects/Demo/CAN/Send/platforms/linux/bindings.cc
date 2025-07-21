#include "../../bindings.hpp"

#include <unistd.h>

#include <iostream>

#include "mcal/linux/can.hpp"
#include "mcal/linux/digital_input.hpp"

namespace mcal {
using namespace lnx;

CanBase demo_can_base{"vcan0"};
DigitalInput button{"Button"};

}  // namespace mcal

namespace bindings {

macfe::periph::CanBase& demo_can_base = mcal::demo_can_base;
macfe::periph::DigitalInput& button = mcal::button;

void TickBlocking(uint32_t ms) {
    usleep(1000 * ms);
}

void Initialize() {
    std::cout << "Initializing Linux" << std::endl;
    mcal::demo_can_base.Setup();
}

}  // namespace bindings