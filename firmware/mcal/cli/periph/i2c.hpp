/// @author Samuel Parent, Hydar Zartash
/// @date 2024-11-21

#pragma once

#include <iostream>

#include "shared/comms/i2c/msg.hpp"
#include "shared/periph/i2c.hpp"

namespace mcal::cli::periph {

class I2C : public shared::periph::I2C {
public:
    I2C(int bus_number) : bus_number_(bus_number) {}
    virtual ~I2C() = default;

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
