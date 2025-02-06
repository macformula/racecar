/// @author Samuel Parent
/// @date 2024-01-21

#pragma once

#include <cstdint>

#include "shared/comms/i2c/msg.hpp"
#include "shared/util/device.hpp"

// Forward declaration of I2CBus to avoid circular dependency
namespace shared::periph {
class I2CBus;
}

namespace shared::device {

class I2CMultiplexer : public util::Device {
public:
    I2CMultiplexer(shared::periph::I2CBus& i2c_bus, uint8_t mux_address)
        : i2c_bus_(i2c_bus), mux_address_(mux_address) {}

    virtual ~I2CMultiplexer() = default;

    virtual void WriteToChannel(uint8_t channel,
                                const shared::i2c::Message& msg) = 0;
    virtual void ReadFromChannel(uint8_t channel,
                                 shared::i2c::Message& msg) = 0;

protected:
    shared::periph::I2CBus& i2c_bus_;
    const uint8_t mux_address_;
};

}  // namespace shared::device
