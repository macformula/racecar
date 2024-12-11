#pragma once

#include <concepts>
#include <cstdint>
#include <format>

namespace shared::can {

struct RawMessage {
    RawMessage() = default;
    RawMessage(uint32_t id_, bool is_extended_frame_, uint8_t data_length_,
               const uint8_t data_[8]);

    uint32_t id;
    bool is_extended_frame;
    uint8_t data_length;
    uint8_t data[8];
};

// TxMessage concept is required by Bus.Send()
template <typename T>
concept TxMessage = requires(const T msg) {
    { msg.pack() } -> std::same_as<RawMessage>;
};

// Used in the generated code to unpack from / pack into a RawMessage
template <typename T>
inline T unpack_right_shift(uint8_t value, uint8_t shift, uint8_t mask) {
    return static_cast<T>(static_cast<T>(value & mask) >> shift);
}

template <typename T>
inline T unpack_left_shift(uint8_t value, uint8_t shift, uint8_t mask) {
    return static_cast<T>(static_cast<T>(value & mask) << shift);
}

template <typename T>
inline uint8_t pack_left_shift(T value, uint8_t shift, uint8_t mask) {
    return static_cast<uint8_t>(static_cast<uint8_t>(value << shift) & mask);
}

template <typename T>
inline uint8_t pack_right_shift(T value, uint8_t shift, uint8_t mask) {
    return static_cast<uint8_t>(static_cast<uint8_t>(value >> shift) & mask);
}

}  // namespace shared::can

// Convert RawMessage to string with `std::format`
template <>
struct std::formatter<shared::can::RawMessage> {
    constexpr auto parse(auto& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const shared::can::RawMessage& msg, FormatContext& ctx) const {
        std::string str = std::format("[{:02X}]", msg.id);
        for (int i = 0; i < msg.data_length; ++i) {
            str += std::format(" {:02X}", msg.data[i]);
        }

        return std::format_to(ctx.out(), "{}", str);
    }
};