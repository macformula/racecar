#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <fmt/core.h>

namespace macfe::can {

struct RawMessage {
    RawMessage() = default;
    RawMessage(uint32_t id_, bool is_extended_frame_, uint8_t data_length_,
               const uint8_t data_[8]);
    RawMessage(uint32_t id_, bool is_extended_frame_, uint8_t data_length_,
               const std::array<uint8_t, 8> data_);
    RawMessage(const RawMessage& other);

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

}  // namespace macfe::can

// Convert RawMessage to string with `fmt::format`
template <>
struct fmt::formatter<macfe::can::RawMessage> {
    constexpr auto parse(auto& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const macfe::can::RawMessage& msg, FormatContext& ctx) const {
        std::string str = fmt::format("[{:02X}]", msg.id);
        for (int i = 0; i < msg.data_length; ++i) {
            str += fmt::format(" {:02X}", msg.data[i]);
        }

        return fmt::format_to(ctx.out(), "{}", str);
    }
};