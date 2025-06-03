#include "suspension.hpp"

#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"

namespace suspension {

static float travel3 = 0;
static float travel4 = 0;

static void Measure(void) {
    // todo: update the mapping
    travel3 = bindings::suspension_travel3.ReadVoltage();
    travel4 = bindings::suspension_travel4.ReadVoltage();
}

void task_10hz(generated::can::VehBus& veh_can) {
    Measure();

    generated::can::TxSuspensionTravel34 suspension_msg{
        .stp3 = static_cast<uint8_t>(travel3),
        .stp4 = static_cast<uint8_t>(travel4),
    };
    veh_can.Send(suspension_msg);
}

}  // namespace suspension