/// @author Samuel Parent, Hydar Zartash
/// @date 2024-11-21

#pragma once

#include "shared/comms/i2c/msg.hpp"
#include "shared/util/peripheral.hpp"
#include "shared/device/i2c_mux.hpp"
namespace shared::periph {

class I2CBus : public util::Peripheral {
public:
    virtual void Write(const shared::i2c::Message& msg) = 0;
    virtual void Read(shared::i2c::Message& msg) = 0;

protected:
    virtual ~I2CBus() = default;
};

class I2CBusMultiplexed : public I2CBus {
public:
    I2CBusMultiplexed(shared::device::I2CMultiplexer& i2c_mux, uint8_t channel)
        : i2c_mux_(i2c_mux), channel_(channel) {}
    virtual void Write(const shared::i2c::Message& msg) {
        i2c_mux_.WriteToChannel(channel_, msg);
    }

    virtual void Read(shared::i2c::Message& msg) {
        i2c_mux_.ReadFromChannel(channel_, msg);
    }

private:
    shared::device::I2CMultiplexer& i2c_mux_;
    uint8_t channel_;
};

}  // namespace shared::periph
