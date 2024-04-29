/// @author Samuel Parent
/// @date 2024-01-16

#include <stdint.h>
#include <cstdint>

#include "generated/can/can_messages.h"
#include "generated/can/msg_registry.h"
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

const char* time = __TIME__;

int main(void) {
    bindings::Initialize();
    uint32_t tick_duration = 100;

    generated::can::TempSensors temp_sens_msg{};
    generated::can::TempSensorsReply temp_sens_msg_reply{};

    while (true) {
        veh_can_bus.Update();
        veh_can_bus.Read(temp_sens_msg);

        temp_sens_msg_reply.sensor1 = static_cast<int8_t>((time[3] - '0')*0x10 + (time[4] - '0'));
        temp_sens_msg_reply.sensor2 = static_cast<int8_t>((time[0] - '0')*0x10 + (time[1] - '0'));
        temp_sens_msg_reply.sensor3 = static_cast<int8_t>(temp_sens_msg.tick_timestamp);
        temp_sens_msg_reply.sensor4 = temp_sens_msg.sensor4;
        temp_sens_msg_reply.sensor5 = temp_sens_msg.sensor5;
        temp_sens_msg_reply.sensor6 = temp_sens_msg.sensor6;

        veh_can_bus.Send(temp_sens_msg_reply);

        bindings::TickBlocking(tick_duration);
    }

    return 0;
}