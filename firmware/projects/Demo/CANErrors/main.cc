/// @author Luai Bashar
/// @date 2024-11-05

#include <cstdint>

#include "app.h"
#include "bindings.h"
#include "shared/comms/can/can_bus.h"
#include "generated/can/error_msg_registry.h"

// Initializing can bus that sends the error message
generated::can::ErrorMsgRegistry error_can_registry{};
shared::can::CanBus error_can_bus{
    bindings::error_can_base,
    error_can_registry,
};

int main(void) {
    // Initialize the CLI and milliseconds to repeat by
    bindings::Initialize();
    uint32_t tick_duration = 100;

    // Initialize the error object that sets/sends each error
    ErrorHandler error_message{};

    while (true) {
        error_can_bus.Update();

        // Test #1: Iterate through each error, setting it and sending it
        for (int i = 0; i < 64; i++) {
            Error error = static_cast<Error>(i);
            error_message.SetError(error);
            error_message.SendError(error_can_bus);
        }

        // Test #2: Set random errors using the enum
        error_message.SetError(Error::kError0);
        error_message.SetError(Error::kError5);
        error_message.SetError(Error::kError10);
        error_message.SetError(Error::kError15);
        error_message.SetError(Error::kError20);
        error_message.SendError(error_can_bus);

        // Wait for 100ms before repeating the process
        bindings::TickBlocking(tick_duration);
    }

    return 0;
}