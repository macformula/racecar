/// @author Samuel Parent
/// @date 2023-01-17

#pragma once

#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/raw_can_msg.h"
#include "shared/comms/can/bus_manager.h"

namespace generated::can {

constexpr uint16_t kNumCanRxMesssages = 1;

class TmsBroadcast : public shared::comms::can::CanTxMsg {
private:
    constexpr uint32_t kCanId = 0x1839F380;
    constexpr uint8_t kDlc = 8;
    constexpr bool kIsExtFrame = true;

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

    template<uint16_t num_rx_messages>
    class BusManager;
};

class DebugLedOverride : public shared::comms::can::CanRxMsg {
private:
    constexpr uint32_t kCanId = 0x645;
    constexpr uint8_t kDlc = 1;
    constexpr bool kIsExtFrame = true;

    void Unpack(const shared::comms::can::RawCanMsg& raw_msg) {
        if (raw_msg.header.id == kCanId) {
            set_green_led = static_cast<uint8_t>(raw_msg.data[0] & 0b00000001);
            set_red_led = static_cast<uint8_t>(raw_msg.data[0] & 0b00000010);
        }
    }
    
public:
    DebugLedOverride(){};
    ~DebugLedOverride(){};
    
    bool set_green_led = false;
    bool set_red_led = false;

    template<uint16_t num_rx_messages>
    class BusManager;
};

} // namespace generated::can