#include "brakelight.hpp"

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/gpio.hpp"

namespace brake_light {
using macfe::periph::DigitalOutput;

void Controller::task_100hz(generated::can::VehBus& veh_can) {
    auto msg = veh_can.GetRxLvCommand();

    if (msg.has_value()) {
        periph.brake_light_en->Set(msg->BrakeLightEnable());
    } else {
        periph.brake_light_en->SetHigh();
    }
}

}  // namespace brake_light