/// @author Samuel Parent
/// @date 2024-01-21

#pragma once

#include <concepts>
#include <cstdint>

#include "shared/comms/i2c/msg.hpp"
#include "shared/periph/i2c.hpp"
#include "shared/util/device.hpp"

namespace shared::device {

template <typename ChannelT>
    requires std::unsigned_integral<std::underlying_type_t<ChannelT>> ||
             std::unsigned_integral<ChannelT>
class I2CMultiplexer : public util::Device {
public:
    I2CMultiplexer(shared::periph::I2CBus& i2c_bus, uint8_t mux_address)
        : i2c_bus_(i2c_bus), mux_address_(mux_address) {}

    virtual ~I2CMultiplexer() = default;

    virtual void WriteToChannel(ChannelT channel, const i2c::Message& msg) = 0;
    virtual void ReadFromChannel(ChannelT channel, i2c::Message& msg) = 0;

protected:
    periph::I2CBus& i2c_bus_;
    const uint8_t mux_address_;
};

template <typename ChannelT>
    requires std::unsigned_integral<std::underlying_type_t<ChannelT>> ||
             std::unsigned_integral<ChannelT>
class I2CBusMultiplexed : public periph::I2CBus {
public:
    I2CBusMultiplexed(I2CMultiplexer<ChannelT>& i2c_mux, ChannelT channel)
        : i2c_mux_(i2c_mux), channel_(channel) {}
    virtual void Write(const i2c::Message& msg) {
        i2c_mux_.WriteToChannel(channel_, msg);
    }

    virtual void Read(i2c::Message& msg) {
        i2c_mux_.ReadFromChannel(channel_, msg);
    }

private:
    I2CMultiplexer<ChannelT>& i2c_mux_;
    ChannelT channel_;
};

}  // namespace shared::device
