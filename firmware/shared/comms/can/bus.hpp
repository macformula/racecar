#pragma once

#include "shared/comms/can/msg.hpp"
#include "shared/periph/can.hpp"

namespace shared::can {

class Bus {
public:
    Bus(periph::CanBase& can_base);

    template <TxMessage T>
    void Send(const T& msg);

private:
    periph::CanBase& can_base_;

    virtual void AddMessage(const RawMessage& msg, uint32_t timestamp) = 0;
    friend class shared::periph::CanBase;
};

// Must define this function in the header file since it is templated
template <TxMessage T>
void Bus::Send(const T& msg) {
    RawMessage raw_msg = msg.pack();
    can_base_.Send(raw_msg);
}

}  // namespace shared::can