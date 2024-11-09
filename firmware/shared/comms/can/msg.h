/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <concepts>
#include <cstdint>
#include <cstring>
#include <format>

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

// Convert RawMessage to string with `std::format`
template <>
struct std::formatter<shared::can::RawMessage> {
    constexpr auto parse(auto& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const shared::can::RawMessage& msg, FormatContext& ctx) const {
        std::string str = std::format("[{:02X}]", msg.id_);
        for (int i = 0; i < msg.data_length; ++i) {
            str += std::format(" {:02X}", msg.data_[i]);
        }

        return std::format_to(ctx.out(), "{}", str);
    }
};