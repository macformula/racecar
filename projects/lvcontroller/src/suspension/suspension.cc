#include "bindings.hpp"
#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"
#include "suspension.hpp"

namespace suspension {

static float v3 = 0;
static float v4 = 0;
static float travel3 = 0;
static float travel4 = 0;
inline constexpr float sensorSupply_V =
    5.0f;  // set to 5V, change depending on what the sensor runs at

static void Measure(void) {
    v3 = bindings::suspension_travel3.ReadVoltage();
    v4 = bindings::suspension_travel4.ReadVoltage();

    // Map v3, v4 to position (mm)
    travel3 = macfe::positionFromVoltage(v3, sensorSupply_V);
    travel4 = macfe::positionFromVoltage(v4, sensorSupply_V);
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