#include "suspension.hpp"

#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"

namespace suspension {

static const float SENSOR_SUPPLY_V = 3.3f;
static float travel1 = 0.0f;
static float travel2 = 0.0f;

static void Measure(macfe::periph::AnalogInput& suspension_travel1,
                    macfe::periph::AnalogInput& suspension_travel2) {
    float v1 = suspension_travel1.ReadVoltage();
    float v2 = suspension_travel2.ReadVoltage();

    travel1 = macfe::sls095::VoltToMillimeter(v1, SENSOR_SUPPLY_V);
    travel2 = macfe::sls095::VoltToMillimeter(v2, SENSOR_SUPPLY_V);
}

void ReadAndSendTravel(generated::can::VehBus& veh_can,
                       macfe::periph::AnalogInput& suspension_travel1,
                       macfe::periph::AnalogInput& suspension_travel2) {
    Measure(suspension_travel1, suspension_travel2);

    generated::can::TxSuspensionTravel12 suspension_msg{travel1, travel2};
    veh_can.Send(suspension_msg);
}

}  // namespace suspension
