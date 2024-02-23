/// @author Samuel Parent
/// @date 2024-01-16

#include "mcal/raspi/periph/can.h"
#include "shared/comms/can/bus_manager.h"
#include "generated/can_messages.h"

namespace bindings {

constexpr std::string can_iface = "vcan0";

shared::periph::CanBase veh_can = RaspiCanBase{can_iface};

void Initialize () {
    veh_can.Setup();
    std::cout << "Initializing raspi..." << std::endl;
}

} // namespace bindings