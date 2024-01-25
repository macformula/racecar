/// @author Samuel Parent
/// @date 2024-01-16

#include "bindings.h"

namespace bindings {
mcal::periph::CanBase veh_can{"can0"};

void Initialize () {
    veh_can.Setup();
    std::cout << "Initializing raspi..." << std::endl;
}
} // namespace bindings