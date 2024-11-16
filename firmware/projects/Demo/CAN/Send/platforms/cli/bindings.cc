/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <thread>

#include "mcal/cli/periph/can.h"
#include "mcal/cli/periph/gpio.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"

namespace mcal {
using namespace cli::periph;

CanBase demo_can_base{"vcan0"};
DigitalInput button{"Button"};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& demo_can_base = mcal::demo_can_base;
shared::periph::DigitalInput& button = mcal::button;

void TickBlocking(uint32_t ticks) {
    std::chrono::milliseconds duration(ticks);

    std::this_thread::sleep_for(duration);
}

void Initialize() {
    mcal::demo_can_base.Setup();
    std::cout << "Initializing CLI..." << std::endl;
}
}  // namespace bindings