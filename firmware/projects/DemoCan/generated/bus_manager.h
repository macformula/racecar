/// @author Samuel Parent
/// @date 2023-01-23

#include "can_messages.h"
#include "shared/comms/can/can_msg.h"
#include "shared/periph/can.h"
#include <array>
#include <iomanip>

template <shared::periph::CanBase CanBase>
class BusManager {
private:
    using RawCanMsg = shared::comms::can::RawCanMsg;
    using CanMsg = shared::comms::can::CanMsg;

    constexpr static int kMaxMsgQueueLen = 100;

    CanBase& can_base_;

    TmsBroadcast tms_broadcast_;

    shared::comms::can::RawCanMsg tx_queue_[kMaxMsgQueueLen] = {0};

public:
    BusManager(CanBase& can_base): can_base_(can_base){};

    template<
    void Send(TmsBroadcast msg) {
        RawCanMsg raw_msg;

        msg.Pack(raw_msg);

        // std::cout <<
        //     "header:" << std::setw(3) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.header.id) << "\n"
        //     "[0]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[0]) << "\n" <<
        //     "[1]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[1]) << "\n" <<
        //     "[2]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[2]) << "\n" <<
        //     "[3]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[3]) << "\n" <<
        //     "[4]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[4]) << "\n" <<
        //     "[5]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[5]) << "\n" <<
        //     "[6]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[6]) << "\n" <<
        //     "[7]:" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(raw_msg.data[7]) << "\n" << std::endl;


        can_base_.Send(raw_msg);
    }

    void Read(TmsBroadcast& msg) {
        msg = tms_broadcast_;
        return;
    }
};