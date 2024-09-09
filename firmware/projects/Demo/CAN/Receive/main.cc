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
    uint32_t tick_duration = 100;
    uint16_t last_msg_count = -1;

    generated::can::ButtonStatus btn_msg{};

    while (true) {
        veh_can_bus.Update();
        veh_can_bus.Read(btn_msg);

        std::cout << btn_msg.msg_count << "-" << last_msg_count;
        if (btn_msg.msg_count == last_msg_count) {
            // continue;
        } else {
            last_msg_count = btn_msg.msg_count;
        };

        bindings::indicator.Set(btn_msg.state);

        // veh_can_bus.Send(temp_sens_msg_reply);

        bindings::TickBlocking(tick_duration);
    }

    return 0;
}