/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.h"
#include "generated/can/demobus_bus.hpp"
#include "generated/can/demobus_messages.hpp"

using namespace generated::can;

DemobusBus can_bus{bindings::demo_can_base};

int main(void) {
    bindings::Initialize();
    uint32_t interval_ms = 5;

    while (true) {
        TxButtonStatus msg{
            .state = bindings::button.Read(),
        };
        can_bus.Send(msg);
        bindings::TickBlocking(interval_ms);
    }

    return 0;
}