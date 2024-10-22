/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.h"
#include "generated/can/demobus_can_messages.h"
#include "generated/can/demobus_msg_registry.h"
#include "shared/comms/can/can_bus.h"

generated::can::DemobusMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

int main(void) {
    bindings::Initialize();
    uint16_t last_msg_count = -1;

    generated::can::ButtonStatus btn_msg{};

    while (true) {
        veh_can_bus.Update();
        veh_can_bus.Read(btn_msg);

        // Read() will return the latest message. Do nothing if the message has
        // been seen already
        if (btn_msg.msg_count == last_msg_count) {
            continue;
        }

        last_msg_count = btn_msg.msg_count;
        bindings::indicator.Set(btn_msg.state);
    }

    return 0;
}