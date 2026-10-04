#include "suspension.hpp"

#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"

namespace suspension {
float SENSOR_SUPPLY_V = 3.3f;

void task_10hz(generated::can::VehBus& veh_can,
               macfe::periph::AnalogInput& suspension_travel3,
               macfe::periph::AnalogInput& suspension_travel4) {
    auto travel3 = macfe::sls095::VoltToMillimeter(
        suspension_travel3.ReadVoltage(), SENSOR_SUPPLY_V);
    auto travel4 = macfe::sls095::VoltToMillimeter(
        suspension_travel4.ReadVoltage(), SENSOR_SUPPLY_V);

    generated::can::TxSuspensionTravel34 suspension_msg{travel3, travel4};
    veh_can.Send(suspension_msg);
}

}  // namespace suspension
