/// @author Samuel Parent
/// @date 2024-01-16

#include <chrono>
#include <thread>

#include "lib/periph/can.hpp"
#include "mcal/raspi/can.hpp"

namespace mcal {
using namespace raspi;

CanBase veh_can_base{"vcan0"};
}  // namespace mcal

namespace bindings {
macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    mcal::veh_can_base.Setup();
    std::cout << "Initializing raspi..." << std::endl;
}
}  // namespace bindings