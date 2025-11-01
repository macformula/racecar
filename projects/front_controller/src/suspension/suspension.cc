#include "suspension.hpp"

#include "../../front_controller/include/bindings.hpp"
#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"

namespace suspension {

static const float SENSOR_SUPPLY_V = 3.3f;
static float travel1 = 0.0f;
static float travel2 = 0.0f;

static void Measure(void) {
    float v1 = bindings::suspension_travel1.ReadVoltage();
    float v2 = bindings::suspension_travel2.ReadVoltage();

    travel1 = macfe::sls095::VoltToMillimeter(v1, SENSOR_SUPPLY_V);
    travel2 = macfe::sls095::VoltToMillimeter(v2, SENSOR_SUPPLY_V);
}

void task_10hz(generated::can::VehBus& veh_can) {
    Measure();

    generated::can::TxSuspensionTravel34 suspension_msg{travel1, travel2};
    veh_can.Send(suspension_msg);
}

}  // namespace suspension
