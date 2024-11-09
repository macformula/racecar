/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <cstdint>

// #include "shared/comms/can/bus.h"
#include "shared/comms/can/msg.h"
#include "shared/util/peripheral.h"

// Need to forware declare Bus to avoid circular dependency
namespace shared::can {
class Bus;
}  // namespace shared::can

namespace shared::periph {

class CanBase : public util::Peripheral {
public:
    virtual void Send(const shared::can::RawMessage&) = 0;
    void Receive(can::RawMessage msg);

private:
    void RegisterBus(can::Bus* bus);
    virtual uint32_t GetTimestamp() = 0;

    can::Bus* bus_;

    friend class can::Bus;
};

}  // namespace shared::periph