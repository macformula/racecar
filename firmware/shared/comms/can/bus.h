/// @author Samuel Parent
/// @date 2024-02-09

#pragma once

#include "shared/comms/can/msg.h"
#include "shared/periph/can.h"

namespace shared::can {

class Bus {
public:
    Bus(periph::CanBase& can_base);

    void Send(RawMessage msg);

private:
    periph::CanBase& can_base_;

    virtual void AddMessage(const RawMessage& msg, uint32_t timestamp) = 0;
    friend class shared::periph::CanBase;
};

}  // namespace shared::can