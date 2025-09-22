/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.hpp"
#include "can/bus.hpp"
#include "generated/can/demobus_bus.hpp"
#include "generated/can/demobus_messages.hpp"

using namespace generated::can;

DemobusBus demo_can_bus{bindings::demo_can_base};

int main(void) {
    bindings::Initialize();

    while (true) {
        auto btn_msg = demo_can_bus.PopRxButtonStatus();

        // We're not guaranteed to have message available, so we need to check.
        if (btn_msg.has_value()) {
            // Unpack the value from the optional
            RxButtonStatus msg = btn_msg.value();
            bindings::indicator.Set(msg.State());
        }
    }

    return 0;
}