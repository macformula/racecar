/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
#include "shared/comms/can/can_bus.h"
#include "shared/os/tick.h"

namespace os {
extern void TickBlocking(uint32_t ticks);
}

namespace bindings {
extern shared::periph::CanBase& veh_can_base;
extern void Initialize();
}  // namespace bindings

generated::can::VehMsgRegistry veh_can_registry{};

shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

int main(void) {
    bindings::Initialize();
    uint32_t tick_duration = 100;

    generated::can::TempSensors temp_sens_msg{};

    while (true) {
        os::TickBlocking(tick_duration);

        veh_can_bus.Update();

        temp_sens_msg.sensor1++;
        temp_sens_msg.sensor2++;
        temp_sens_msg.sensor3++;
        temp_sens_msg.sensor4++;
        temp_sens_msg.sensor5++;
        temp_sens_msg.sensor6++;

        veh_can_bus.Send(temp_sens_msg);
    }

    return 0;
}