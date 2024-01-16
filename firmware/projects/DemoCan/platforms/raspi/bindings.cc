/// @author Samuel Parent
/// @date 2024-01-16

#include "bindings.h"

namespace bindings {

mcal::periph::CanBase veh_can_base{"vcan0"};

void Initialize() {
    veh_can_base.Setup();
    std::cout << "Initializing raspi..." << std::endl;
}

}  // namespace bindings