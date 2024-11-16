/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.h"
#include "generated/can/demobus_bus.h"
#include "generated/can/demobus_messages.h"
#include "shared/comms/can/bus.h"

using namespace generated::can;

DemobusBus demo_can_bus{bindings::demo_can_base};

int main(void) {
    bindings::Initialize();

    while (true) {
        auto btn_msg = demo_can_bus.PopRxButtonStatus();

        if (btn_msg.has_value()) {
            auto msg = btn_msg.value();
            bindings::indicator.Set(msg.State());
        }
    }

    return 0;
}