/// @author Samuel Parent
/// @date 2024-01-13

#pragma once

#include <stdint.h>

#include <algorithm>
#include <iostream>

namespace shared::can {

constexpr uint8_t kMaxMsgBytes = 8;

using CanId = uint32_t;

struct CanHeader {
    CanId id;
    uint8_t data_len;
    bool is_extended_frame;
};

struct RawCanMsg {
    CanHeader header;
    uint8_t data[kMaxMsgBytes] = {0};

    void Copy(const shared::can::RawCanMsg& other) noexcept {
        header.id = other.header.id;
        header.data_len = other.header.data_len;
        header.is_extended_frame = other.header.is_extended_frame;

        std::copy(std::begin(other.data), std::end(other.data),
                  std::begin(data));
    }
};

}  // namespace shared::can
