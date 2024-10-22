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
    uint32_t interval_ms = 50;
    uint16_t msg_count = 0;

    generated::can::ButtonStatus btn_msg{};

    while (true) {
        veh_can_bus.Update();

        btn_msg.state = bindings::button.Read();
        btn_msg.msg_count = ++msg_count;

        veh_can_bus.Send(btn_msg);
        bindings::TickBlocking(interval_ms);
    }

    return 0;
}