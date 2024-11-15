/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <concepts>
#include <cstdint>
#include <format>

namespace shared::can {

struct RawMessage {
    RawMessage(uint32_t id_, uint8_t data_length_, const uint8_t data_[8]);

    uint32_t id;
    uint8_t data_length;
    uint8_t data[8];
};

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