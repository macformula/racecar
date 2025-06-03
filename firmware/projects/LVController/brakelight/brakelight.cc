#include "brakelight.hpp"

#include "../bindings.hpp"

namespace brake_light {

void task_100hz(generated::can::VehBus& veh_can) {
    auto msg = veh_can.GetRxBrakeLight();

    if (msg.has_value()) {
        bindings::brake_light_en.Set(msg->Enable());
    } else {
        bindings::brake_light_en.SetHigh();
    }
}

}  // namespace brake_light