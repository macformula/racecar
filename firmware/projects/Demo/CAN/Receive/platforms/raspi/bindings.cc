/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <thread>

#include "mcal/raspi/periph/can.hpp"
#include "shared/periph/can.hpp"

namespace mcal {
using namespace raspi::periph;

CanBase veh_can_base{"vcan0"};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    mcal::veh_can_base.Setup();
    std::cout << "Initializing raspi..." << std::endl;
}
}  // namespace bindings