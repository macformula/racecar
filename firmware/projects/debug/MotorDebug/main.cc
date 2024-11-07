#include <stdint.h>

#include "bindings.h"
#include "generated/can/vehicle_can_messages.h"
#include "generated/can/vehicle_msg_registry.h"
#include "projects/debug/MotorDebug/generated/can/vehicle_msg_registry.h"
#include "shared/comms/can/can_bus.h"
#include "shared/periph/can.h"

namespace bindings {

extern shared::periph::CanBase& vehicle_can_base;

}  // namespace bindings

generated::can::VehicleMsgRegistry veh_can_registry{};
shared::can::CanBus vehicle_can_bus{
    bindings::vehicle_can_base,
    veh_can_registry,
};

generated::can::APPS pedal_message{};

bool debug_led_state = true;

int main() {
    bindings::Initialize();

    while (true) {
        uint32_t accel_pedal_pos_1 = bindings::accel_pedal_1.Read();
        uint32_t accel_pedal_pos_2 = bindings::accel_pedal_2.Read();

        // // Send over CAN
        pedal_message.apps1 = static_cast<uint16_t>(accel_pedal_pos_1);
        pedal_message.apps2 = static_cast<uint16_t>(accel_pedal_pos_2);
        vehicle_can_bus.Send(pedal_message);

        bindings::DelayMS(100);
        bindings::debug_led.Set(debug_led_state);
        debug_led_state = !debug_led_state;
    }

    return 0;
}