#pragma once

#include "can/bus.hpp"
#include "periph/can.hpp"

namespace macfe::can {

class Bus {
public:
    Bus(periph::CanBase& can_base);

    template <TxMessage T>
    void Send(const T& msg);

private:
    periph::CanBase& can_base_;

    virtual void AddMessage(const RawMessage& msg, uint32_t timestamp) = 0;
    friend class macfe::periph::CanBase;
};

// Must define this function in the header file since it is templated
template <TxMessage T>
void Bus::Send(const T& msg) {
    RawMessage raw_msg = msg.pack();
    can_base_.Send(raw_msg);
}

}  // namespace macfe::can
