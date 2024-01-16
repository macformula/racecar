/// @author Samuel Parent
/// @date 2024-01-13

#ifndef SHARED_COMMS_CAN_RAW_CAN_MSG_H_
#define SHARED_COMMS_CAN_RAW_CAN_MSG_H_

#include <stdint.h>

namespace shared::comms::can {

#define CAN_MSG_BYTES 8;

typedef struct can_header {
    uint32_t can_id;
    bool is_extended_frame;
};

typedef struct raw_can_msg {
    can_header can_hdr;
    uint8_t data_len;
    uint8_t bytes[CAN_MSG_BYTES];
};

} // namespace shared::comms::can

#endif  // SHARED_COMMS_CAN_RAW_CAN_MSG_H_