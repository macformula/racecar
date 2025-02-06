/// @author Samuel Parent, Hydar Zartash
/// @date 2024-11-21

#pragma once

#include <iostream>

#include "shared/comms/i2c/msg.hpp"
#include "shared/periph/i2c.hpp"

namespace mcal::cli::periph {

class I2CBus : public shared::periph::I2CBus {
public:
    I2CBus(int bus_number) : bus_number_(bus_number) {}
    virtual ~I2CBus() = default;

    virtual void Write(const shared::i2c::Message& msg) override {
        std::cout << "I2C " << bus_number_ << " - " << msg << "\n";
    };

    virtual void Read(shared::i2c::Message& msg) override {
        std::cout << "I2C " << bus_number_ << " - " << msg << "\n";
    };

protected:
    int bus_number_;
};

}  // namespace mcal::cli::periph
