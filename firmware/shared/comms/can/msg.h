/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <concepts>
#include <cstdint>
#include <cstring>

namespace shared::can {

struct RawMessage {
    RawMessage(uint32_t id, uint8_t data_length, const uint8_t data[8]);

    uint32_t id_;
    uint8_t data_length;
    uint8_t data_[8];
};

// TxMessage concept is required by Bus.Send()
template <typename T>
concept TxMessage = requires(const T msg) {
    { msg.encode() } -> std::same_as<RawMessage>;
};

// We don't need an RxMessage cocept since no function takes in an arbitrary
// RxMessage.

}  // namespace shared::can
