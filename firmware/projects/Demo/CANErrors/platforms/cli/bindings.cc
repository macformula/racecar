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

    // Interface to set/send all possible errors for the system
    class Errors {
        private:
            // Object that holds a 64 bit int, each bit representing an error
            generated::can::TMS_ERROR error_msg{};
        public:
            // Sets the error based on the error index given
            void setError(uint32_t error) {
                if (error < 0 || error > 63) {
                    std::cerr << "Error index must be between 0 and 63!" << std::endl;
                    return;
                }

                error_msg.errors |= (1ULL << error);
            }

            // Sends the error message through the provided CAN bus
            void sendError(shared::can::CanBus error_can_bus) {
                error_can_bus.Send(error_msg);

                // Reset after a send to not send duplicate errors
                resetError();
            }

            // Resets all errors back to untriggered
            void resetError() {
                error_msg.errors = 0;
            }
    };
}