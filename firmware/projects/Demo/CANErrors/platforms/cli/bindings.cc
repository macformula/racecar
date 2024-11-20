/// @author Luai Bashar
/// @date 2024-11-05

#include <chrono>
#include <thread>

#include "mcal/cli/periph/can.h"
#include "shared/periph/can.h"

#include "../../generated/can/error_can_messages.h"
#include "shared/comms/can/can_bus.h"

namespace mcal {
    using namespace cli::periph;

    CanBase error_can_base{"vcan0"};
}

namespace bindings {
    shared::periph::CanBase& error_can_base = mcal::error_can_base;

    // Simulates a sleep, waiting for inputted ticks ms
    void TickBlocking(uint32_t ticks) {
        std::chrono::milliseconds duration(ticks);

        std::this_thread::sleep_for(duration);
    }

    // Initializes the can/CLI outputs
    void Initialize() {
        mcal::error_can_base.Setup();
        std::cout << "Initializing CLI..." << std::endl;
    }
}