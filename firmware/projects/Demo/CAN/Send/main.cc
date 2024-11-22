/// @author Samuel Parent
/// @date 2024-01-16

#include <cstdint>

#include "bindings.h"
#include "generated/can/demobus_bus.h"
#include "generated/can/demobus_messages.h"

using namespace generated::can;

DemobusBus can_bus{bindings::demo_can_base};

int main(void) {
    bindings::Initialize();
    uint32_t interval_ms = 50;

    while (true) {
        TxButtonStatus msg{
            .state = bindings::button.Read(),
        };
        can_bus.Send(msg);
        bindings::TickBlocking(interval_ms);
    }

    return 0;
}