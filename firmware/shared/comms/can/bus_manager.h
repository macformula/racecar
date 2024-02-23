/// @author Samuel Parent
/// @date 2024-02-09

#pragma once

#include <cstdint>

#include "etl/unordered_map.h"
#include "shared/periph/can.h"
#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/raw_can_msg.h"

namespace shared::comms::can {

template<uint16_t num_rx_messages>
class BusManager {
private:
    constexpr static int kMaxMsgQueueLen = 100;

    using CanId = uint32_t;

    shared::periph::CanBase& can_base_;

    RawCanMsg rx_queue_[kMaxMsgQueueLen] = {0};

    const etl::unordered_map<CanId, CanRxMsg, num_rx_messages>& rx_msg_registry_;
public:
    BusManager(
        CanBase& can_base, 
        const etl::unordered_map<CanId, CanRxMsg, num_rx_messages>& rx_msg_registry) : 
        can_base_(can_base),
        rx_msgs_registry_(rx_msg_registry) {};

    void Send(CanTxMsg msg) {
        RawCanMsg raw_msg;

        msg.Pack(raw_msg);

        can_base_.Send(raw_msg);
    }

    void Update() {
        can_base_.ReadQueue(rx_queue_);

        for (const auto& raw_msg : rx_queue_) {
            rx_msg_registry_[raw_msg.header.id].Unpack(raw_msg);
        }

        return;
    }

    
    void Read(CanRxMsg& rx_msg) {
        rx_msg = rx_msg_registry_[rx_msg.GetId()];
    }
};

} // shared::comms::can