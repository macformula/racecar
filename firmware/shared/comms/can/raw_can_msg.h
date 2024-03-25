/// @author Samuel Parent
/// @date 2024-01-13

#pragma once

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

}  // namespace shared::comms::can
