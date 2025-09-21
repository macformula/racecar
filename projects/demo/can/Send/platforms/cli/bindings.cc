/// @author Samuel Parent
/// @date 2024-01-16

#include "../../bindings.hpp"

#include <chrono>
#include <thread>

#include "lib/periph/can.hpp"
#include "lib/periph/gpio.hpp"
#include "mcal/cli/can.hpp"
#include "mcal/cli/gpio.hpp"

namespace mcal {
using namespace cli;

CanBase demo_can_base{"vcan0"};
DigitalInput button{"Button"};
}  // namespace mcal

namespace bindings {
macfe::periph::CanBase& demo_can_base = mcal::demo_can_base;
macfe::periph::DigitalInput& button = mcal::button;

void TickBlocking(uint32_t ticks) {
    std::chrono::milliseconds duration(ticks);

    std::this_thread::sleep_for(duration);
}

void Initialize() {
    mcal::demo_can_base.Setup();
    std::cout << "Initializing CLI..." << std::endl;
}
}  // namespace bindings