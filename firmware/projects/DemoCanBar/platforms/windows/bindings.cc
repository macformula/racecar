/// @author Samuel Parent
/// @date 2024-01-16

#include "mcal/windows/periph/can.h"
#include "shared/periph/can.h"

namespace mcal {
using namespace windows::periph;

CanBase veh_can_base{"vcan0"};
}  // namespace mcal

namespace bindings {
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    mcal::veh_can_base.Setup();
    std::cout << "Initializing windows..." << std::endl;
}
}  // namespace bindings