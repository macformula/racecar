/// @author Samuel Parent
/// @date 2024-01-13

#ifndef SHARED_COMMS_CAN_RAW_CAN_MSG_H_
#define SHARED_COMMS_CAN_RAW_CAN_MSG_H_

#include <stdint.h>

namespace shared::comms::can {

constexpr uint8_t kMaxMsgBytes = 8;

using CanId = uint32_t;

struct CanHeader {
    CanId id;
    uint8_t data_len;
    bool is_extended_frame;
};

struct RawCanMsg {
    CanHeader header;
    uint8_t data[kMaxMsgBytes];
};

} // namespace shared::comms::can

#endif  // SHARED_COMMS_CAN_RAW_CAN_MSG_H_