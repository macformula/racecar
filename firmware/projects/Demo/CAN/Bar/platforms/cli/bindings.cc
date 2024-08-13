/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <thread>

#include "mcal/cli/periph/can.h"
#include "shared/periph/can.h"

namespace mcal {
using namespace cli::periph;

CanBase veh_can_base{"vcan0"};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void TickBlocking(uint32_t ticks) {
    std::chrono::milliseconds duration(ticks);

    std::this_thread::sleep_for(duration);
}

void Initialize() {
    mcal::veh_can_base.Setup();
    std::cout << "Initializing CLI..." << std::endl;
}
}  // namespace bindings