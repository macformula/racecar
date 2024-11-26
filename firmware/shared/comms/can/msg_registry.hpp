/// @author Samuel Parent
/// @date 2024-03-01

#pragma once

#include "shared/comms/can/raw_can_msg.hpp"

namespace shared::can {

class MsgRegistry {
public:
    virtual bool SetMessage(const RawCanMsg&) = 0;
    virtual bool GetMessage(CanRxMsg&) = 0;

protected:
    static inline void Unpack(CanRxMsg* rx_msg, const RawCanMsg& raw_msg) {
        rx_msg->Unpack(raw_msg);
    }

    static inline void Clone(const CanRxMsg* const srd_rx_msg,
                             CanRxMsg& dst_rx_msg) {
        srd_rx_msg->Clone(dst_rx_msg);
    }

    static inline CanId MsgId(const CanRxMsg* const rx_msg) {
        return rx_msg->Id();
    }
};

}  // namespace shared::can