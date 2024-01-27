/// @author Samuel Parent
/// @date 2023-01-23

#include "can_messages.h"
#include "shared/comms/can/can_msg.h"
#include "shared/periph/can.h"
#include <array>

template <shared::periph::CanBase CanBase>
class BusManager {
private:
    using RawCanMsg = shared::comms::can::RawCanMsg;
    using CanMsg = shared::comms::can::CanMsg;

    constexpr static int kMaxMsgQueueLen = 100;

    CanBase can_base_;

    TmsBroadcast tms_broadcast_;

    std::array<shared::comms::can::RawCanMsg&, kMaxMsgQueueLen> tx_queue_;
public:
    BusManager(CanBase& can_base) can_base_(can_base) {};

    void Send(TmsBroadcast msg) {
        RawCanMsg raw_msg;

        msg.Pack(raw_msg);

        can_base_.Send(raw_msg);
    }

    void Read(TmsBroadcast& msg) {
        return tms_broadcast_;
    }
};