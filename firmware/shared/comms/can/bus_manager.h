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
    using RxMessageRegistry = etl::unordered_map<CanId, CanRxMsg, num_rx_messages>;

    shared::periph::CanBase& can_base_;

    RawCanMsg rx_queue_[kMaxMsgQueueLen] = {0};

    RxMessageRegistry& rx_msg_registry_;
public:
    BusManager(CanBase& can_base, 
        RxMessageRegistry& rx_msg_registry) : 
        can_base_(can_base),
        rx_msgs_registry_(rx_msg_registry) {};

    void Send(CanTxMsg& msg) {
        RawCanMsg raw_msg = 0;

        msg.Pack(raw_msg);

        can_base_.Send(raw_msg);
    }

    void Update(CanRxMsg& rx_msg) {
        can_base_.ReadQueue(rx_queue_);

        for (const auto& raw_msg : rx_queue_) {
            rx_msg_registry_[raw_msg.header.id].Unpack(raw_msg);
        }

        return;
    }

    void Read(CanRxMsg& rx_msg) {
        // TODO: Add mutex lock here.
        rx_msg = rx_msg_registry_[rx_msg.GetId()];
        
        return;
    }
};

} // shared::comms::can