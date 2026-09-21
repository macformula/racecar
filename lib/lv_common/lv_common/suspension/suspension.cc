#include "suspension.hpp"

#include "generated/can/veh_messages.hpp"
#include "sls095.hpp"

namespace suspension {
void Controller::Measure(void) {
    float v3 = periph.suspension_travel3->ReadVoltage();
    float v4 = periph.suspension_travel4->ReadVoltage();

    travel3 = macfe::sls095::VoltToMillimeter(v3, SENSOR_SUPPLY_V);
    travel4 = macfe::sls095::VoltToMillimeter(v4, SENSOR_SUPPLY_V);
}
void Controller::task_10hz(generated::can::VehBus& veh_can) {
    Measure();

    generated::can::TxSuspensionTravel34 suspension_msg{travel3, travel4};
    veh_can.Send(suspension_msg);
}

}  // namespace suspension
