/// @author Samuel Parent
/// @date 2023-03-01

#pragma once

#include "can_messages.h"
#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/msg_registry.h"
#include "shared/comms/can/raw_can_msg.h"
#include "third-party/etl/include/etl/unordered_map.h"

namespace generated::can {

class DemoCanVehMsgRegistry : public shared::comms::can::MsgRegistry {
public:
    DemoCanVehMsgRegistry(){};

    void Unpack(const shared::comms::can::RawCanMsg& raw_msg) {
        rx_msg_map_[raw_msg.header.id]->Unpack(raw_msg);
    }

    void Clone(shared::comms::can::CanRxMsg& rx_msg) {
        rx_msg_map_[rx_msg.Id()]->Clone(rx_msg);
    }

private:
    static constexpr size_t kNumRxMsgs = 1;

    // Can Rx Messages
    DebugLedOverride debug_led_override_;

    etl::unordered_map<shared::comms::can::CanId, shared::comms::can::CanRxMsg*,
                       kNumRxMsgs>
        rx_msg_map_ = {{0x645, &debug_led_override_}};
};

}  // namespace generated::can