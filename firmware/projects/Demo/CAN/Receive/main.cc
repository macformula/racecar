/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.h"
#include "generated/can/demobus_can_messages.hpp"
#include "generated/can/demobus_msg_registry.hpp"
#include "shared/comms/can/can_bus.hpp"

generated::can::DemobusMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

int main(void) {
    bindings::Initialize();

    generated::can::ButtonStatus btn_msg{};

    while (true) {
        veh_can_bus.Update();
        veh_can_bus.Read(btn_msg);

        bindings::indicator.Set(btn_msg.state);
    }

    return 0;
}