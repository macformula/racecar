/// @author Samuel Parent
/// @date 2023-01-17

#pragma once

#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/raw_can_msg.h"

namespace generated::can {

class TmsBroadcast : public shared::comms::can::CanTxMsg {
private:
    static constexpr shared::comms::can::CanId kCanId = 0x1839F380;
    static constexpr uint8_t kDlc = 8;
    static constexpr bool kIsExtFrame = true;

    void Pack(shared::comms::can::RawCanMsg& raw_msg) {
        raw_msg.header.id = kCanId;
        raw_msg.header.data_len = kDlc;
        raw_msg.header.is_extended_frame = kIsExtFrame;

        raw_msg.data[0] = static_cast<uint8_t>(therm_module_num & 0xFF);
        raw_msg.data[1] = static_cast<uint8_t>(num_therm_enabled & 0xFF);
        raw_msg.data[2] = static_cast<uint8_t>(low_therm_value & 0xFF);
        raw_msg.data[3] = static_cast<uint8_t>(high_therm_value & 0xFF);
        raw_msg.data[4] = static_cast<uint8_t>(avg_therm_value & 0xFF);
        raw_msg.data[5] = static_cast<uint8_t>(high_therm_id & 0xFF);
        raw_msg.data[6] = static_cast<uint8_t>(low_therm_id & 0xFF);
        raw_msg.data[7] = static_cast<uint8_t>(checksum & 0xFF);
    }

public:
    uint8_t therm_module_num = 0;
    uint8_t num_therm_enabled = 0;
    int8_t low_therm_value = 0;
    int8_t high_therm_value = 0;
    int8_t avg_therm_value = 0;
    uint8_t high_therm_id = 0;
    uint8_t low_therm_id = 0;
    uint8_t checksum = 0;
};

class DebugLedOverride : public shared::comms::can::CanRxMsg {
private:
    static constexpr shared::comms::can::CanId kCanId = 0x645;
    static constexpr uint8_t kDlc = 1;
    static constexpr bool kIsExtFrame = true;

public:
    void Clone(shared::comms::can::CanRxMsg& rx_msg) {
        DebugLedOverride* p_rx_msg = static_cast<DebugLedOverride*>(&rx_msg);

        p_rx_msg->set_green_led = set_green_led;
        p_rx_msg->set_red_led = set_red_led;
    }

    void Unpack(const shared::comms::can::RawCanMsg& raw_msg) {
        if (raw_msg.header.id == kCanId) {
            set_green_led = static_cast<uint8_t>(raw_msg.data[0] & 0b00000001);
            set_red_led = static_cast<uint8_t>(raw_msg.data[0] & 0b00000010);
        }
    }

    shared::comms::can::CanId Id() {
        return kCanId;
    }

    DebugLedOverride(){};
    ~DebugLedOverride(){};

    bool set_green_led = false;
    bool set_red_led = false;
};

}  // namespace generated::can