/// @author Samuel Parent
/// @date 2024-02-09

#pragma once

#include "shared/comms/can/msg.h"
#include "shared/periph/can.h"

namespace shared::can {

class Bus {
public:
    Bus(periph::CanBase& can_base);

    template <TxMessage T>
    void Send(T msg);

private:
    periph::CanBase& can_base_;

    virtual void AddMessage(const RawMessage& msg, uint32_t timestamp) = 0;
    friend class shared::periph::CanBase;
};

/**
 * @brief Relay the message to the base.
 * @note This is a template method so it must be defined in the header.
 */
template <TxMessage T>
void Bus::Send(T msg) {
    can_base_.Send(msg.encode());
}

}  // namespace shared::can