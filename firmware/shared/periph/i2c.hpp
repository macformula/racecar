/// @author Samuel Parent, Hydar Zartash
/// @date 2024-11-21

#pragma once

#include "shared/comms/i2c/msg.hpp"
#include "shared/util/peripheral.hpp"

namespace shared::periph {

class I2CBus : public util::Peripheral {
public:
    virtual void Write(const shared::i2c::Message& msg) = 0;
    virtual void Read(shared::i2c::Message& msg) = 0;

protected:
    virtual ~I2CBus() = default;
};

}  // namespace shared::periph
