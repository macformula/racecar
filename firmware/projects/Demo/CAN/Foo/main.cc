/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "generated/can/veh_can_messages.h"
#include "generated/can/veh_msg_registry.h"
#include "shared/comms/can/can_bus.h"

namespace bindings {
extern shared::periph::CanBase& veh_can_base;
extern void Initialize();
extern void TickBlocking(uint32_t);
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
        veh_can_bus.Update();

        temp_sens_msg.sensor1++;
        temp_sens_msg.sensor2++;
        temp_sens_msg.sensor3++;
        temp_sens_msg.sensor4++;
        temp_sens_msg.sensor5++;
        temp_sens_msg.sensor6++;

        veh_can_bus.Send(temp_sens_msg);

        bindings::TickBlocking(tick_duration);
    }

    return 0;
}