/// @author Blake Freer
/// @date 2024-11

#pragma once

#include <cstdint>

#include "shared/comms/can/msg.hpp"

namespace shared::can {
class Bus;  // forward declare to avoid circular dependency
}  // namespace shared::can

namespace shared::periph {

class CanBase {
public:
    virtual void Send(const shared::can::RawMessage&) = 0;
    void AddToBus(can::RawMessage msg);

private:
    void RegisterBus(can::Bus* bus);
    virtual uint32_t GetTimestamp() const = 0;

    can::Bus* bus_;

    friend class can::Bus;
};

// Projects can optionally implement this function
void CanErrorHandler(CanBase* can_base);

}  // namespace shared::periph