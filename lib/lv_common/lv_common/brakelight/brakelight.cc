#include "brakelight.hpp"

#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "periph/gpio.hpp"

namespace brake_light {

void task_100hz(generated::can::VehBus& veh_can,
                macfe::periph::DigitalOutput& brake_light_en) {
    auto msg = veh_can.GetRxLvCommand();

    if (msg.has_value()) {
        brake_light_en.Set(msg->BrakeLightEnable());
    } else {
        brake_light_en.SetHigh();
    }
}

}  // namespace brake_light