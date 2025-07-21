/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <thread>

#include "mcal/raspi/can.hpp"
#include "shared/periph/can.hpp"

namespace mcal {
using namespace raspi;

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
    std::cout << "Initializing raspi..." << std::endl;
}
}  // namespace bindings