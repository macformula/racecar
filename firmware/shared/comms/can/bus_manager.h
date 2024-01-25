/// @author Samuel Parent
/// @date 2024-01-13

#ifndef SHARED_COMMS_CAN_BUS_MANAGER_H_
#define SHARED_COMMS_CAN_BUS_MANAGER_H_

#include "shared/periph/can.h"

namespace shared::comms::can {

template<shared::periph::Can can_base>
class BusManager {
private:
    can_base& can_base_;
    struct TmsBroadcast tms_;


public:
    template<shared::periph::CanMsg can_msg>
    void Send(const can_msg& msg) {
        uint8_t data[CAN_MSG_BYTES];
        raw_can_msg raw_msg;
        
        // Pack message into data
        msg.Pack(data);

        raw_msg.bytes = data;
        raw_msg.can_hdr = msg.Header()
        
        can_base_.Send(raw_msg)
    }

    template<shared::periph::CanMsg can_msg>
    Read(can_msg& msg) {
        message_registry_.Read(can_msg);
    }

};

} // namespae shared::comms::can

#endif  // SHARED_COMMS_CAN_BUS_MANAGER_H_