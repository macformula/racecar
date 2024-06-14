/// @author Samuel Parent
/// @date 2024-02-09

#pragma once

#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/msg_registry.h"
#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"

namespace shared::can {

class CanBus {
public:
    CanBus(shared::periph::CanBase& can_base, MsgRegistry& rx_msg_registry)
        : can_base_(can_base), rx_msg_registry_(rx_msg_registry){};

    void Send(CanTxMsg& msg) {
        RawCanMsg raw_msg;

        msg.Pack(raw_msg);

        can_base_.Send(raw_msg);
    }

    void Update() {
        RawCanMsg rx_queue[kMaxMsgQueueLen] = {};

        can_base_.ReadQueue(rx_queue, kMaxMsgQueueLen);

        for (const auto& raw_msg : rx_queue) {
            rx_msg_registry_.SetMessage(raw_msg);
        }
    }

    void Read(CanRxMsg& rx_msg) {
        rx_msg_registry_.GetMessage(rx_msg);
    }

    void ReadWithUpdate(CanRxMsg& rx_msg) {
        Update();
        Read(rx_msg);
    }

private:
    constexpr static int kMaxMsgQueueLen = 100;

    shared::periph::CanBase& can_base_;
    MsgRegistry& rx_msg_registry_;
};

}  // namespace shared::can