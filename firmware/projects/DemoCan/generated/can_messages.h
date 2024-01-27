/// @author Samuel Parent
/// @date 2023-01-17

#include "shared/comms/can/can_msg.h"
#include "shared/comms/can/raw_can_msg.h"

class TmsBroadcast : public shared::comms::can::CanMsg {
private:
    using RawCanMsg = shared::comms::can::RawCanMsg;
    using CanHeader = shared::comms::can::CanHeader;

    const CanHeader header_ = {
        .id = 0x18F390,
        .data_len = 8,
        .is_extended_frame = true,
    };

    void Pack(RawCanMsg& raw_msg) {
        raw_msg.header = header_;

        raw_msg.data[0] = therm_module_num;
        raw_msg.data[1] = num_therm_enabled;
        raw_msg.data[2] = low_therm_value;
        raw_msg.data[3] = high_therm_value;
        raw_msg.data[4] = avg_therm_value;
        raw_msg.data[5] = high_therm_id;
        raw_msg.data[6] = low_therm_id;
        raw_msg.data[7] = checksum;
    }

    void Unpack(const RawCanMsg& raw_msg) {
        therm_module_num = raw_msg.data[0];
        num_therm_enabled = raw_msg.data[1];
        low_therm_value = raw_msg.data[2];
        high_therm_value = raw_msg.data[3];
        avg_therm_value = raw_msg.data[4];
        high_therm_id = raw_msg.data[5];
        low_therm_id = raw_msg.data[6];
        checksum = raw_msg.data[7];
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

    friend class BusManager;
};




