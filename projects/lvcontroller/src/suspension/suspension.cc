#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"
#include "suspension.hpp"

namespace suspension {

static const float SENSOR_SUPPLY_V = = 3.3f;

static void Measure(void) {
    float v3 = bindings::suspension_travel3.ReadVoltage();
    float v4 = bindings::suspension_travel4.ReadVoltage();

    float travel3 = macfe::VoltToMillimeter(v3, sensorSupply_V);
    float travel4 = macfe::VoltToMillimeter(v4, sensorSupply_V);
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
