#pragma once

#include "shared/comms/can/can_bus.h"
#include "generated/can/error_can_messages.h"

namespace bindings {
    extern shared::periph::CanBase& error_can_base;
    extern void Initialize();
    extern void TickBlocking(uint32_t);

    class Errors {
        private:
            generated::can::TMS_ERROR error_msg{};
        public:
            void setError(uint32_t error);
            void sendError(shared::can::CanBus error_can_bus);
            void resetError();
    };
}