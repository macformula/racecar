/// @author Samuel Parent
/// @date 2023-01-17

#pragma once

#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/raw_can_msg.h"
#include "shared/comms/can/bus_manager.h"


class TmsBroadcast : public shared::comms::can::CanTxMsg {
private:
    constexpr uint32_t kCanId = 0x1839F380;
    constexpr uint8_t kDlc = 8;
    constexpr bool kIsExtFrame = true;

    void Pack(shared::comms::can::RawCanMsg& raw_msg) {
        raw_msg.header.id = kCanId;
        raw_msg.header.data_len = kDlc;
        raw_msg.header.is_extended_frame = kIsExtFrame;

        raw_msg.data[0] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[1] = (uint8_t)(num_therm_enabled & 0xFF);
        raw_msg.data[2] = (uint8_t)(low_therm_value & 0xFF);
        raw_msg.data[3] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[4] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[5] = (uint8_t)(therm_module_num & 0xFF);
    }

public:
    uint8_t therm_module_num;
    uint8_t num_therm_enabled;
    int8_t low_therm_value;
    int8_t high_therm_value;
    int8_t avg_therm_value;
    uint8_t high_therm_id;
    uint8_t low_therm_id;
    uint8_t checksum;

    template<uint16_t num_rx_messages>
    class BusManager;
};

class FanOverride : public shared::comms::can::CanRxMsg {
private:
    constexpr uint32_t kCanId = 0x645;
    constexpr uint8_t kDlc = 1;
    constexpr bool kIsExtFrame = true;

    void Unpack(shared::comms::can::RawCanMsg& raw_msg) {
        raw_msg.header.id = kCanId;
        raw_msg.header.data_len = kDlc;
        raw_msg.header.is_extended_frame = kIsExtFrame;

        raw_msg.data[0] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[1] = (uint8_t)(num_therm_enabled & 0xFF);
        raw_msg.data[2] = (uint8_t)(low_therm_value & 0xFF);
        raw_msg.data[3] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[4] = (uint8_t)(therm_module_num & 0xFF);
        raw_msg.data[5] = (uint8_t)(therm_module_num & 0xFF);
    }
    
public:
    bool override_fan_control;
    uint8_t fan_duty_cycle;

    template<uint16_t num_rx_messages>
    class BusManager;
};




