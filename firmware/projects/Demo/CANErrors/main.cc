/// @author Luai Bashar
/// @date 2024-11-05

#include <cstdint>

#include "generated/can/error_can_messages.h"
#include "generated/can/error_msg_registry.h"
#include "shared/comms/can/can_bus.h"

namespace bindings {
    extern shared::periph::CanBase& error_can_base;
    extern void Initialize();
    extern void TickBlocking(uint32_t);
}  // namespace bindings

generated::can::ErrorMsgRegistry error_can_registry{};

shared::can::CanBus error_can_bus{
    bindings::error_can_base,
    error_can_registry,
};

int main(void) {
    bindings::Initialize();
    uint32_t tick_duration = 100;

    // Initialize the error object that holds all 64 errors
    generated::can::TMS_ERROR error_msg{};

    while (true) {
        error_can_bus.Update();

        // Set the 1st error to the opposite value and send
        error_msg.error0 = !error_msg.error0;
        error_can_bus.Send(error_msg);

        // Set the 15th error to the opposite value and send
        error_msg.error15 = !error_msg.error15;
        error_can_bus.Send(error_msg);

        // Set the 33rd error to the opposite value and send
        error_msg.error33 = !error_msg.error33;
        error_can_bus.Send(error_msg);

        // Set the 7th error to the opposite value and send
        error_msg.error7 = !error_msg.error7;
        error_can_bus.Send(error_msg);

        // Set the 63rd error to the opposite value and send
        error_msg.error63 = !error_msg.error63;
        error_can_bus.Send(error_msg);

        bindings::TickBlocking(tick_duration);
    }

    return 0;
}