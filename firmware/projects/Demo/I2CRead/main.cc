#include <cstdint>
#include <iostream>

#include "bindings.hpp"
#include "shared/comms/i2c/msg.hpp"

int main() {
    bindings::Initialize();

    constexpr uint8_t kAddress = 0x2C;

    while (true) {
        uint8_t data1[4] = {};
        auto read_msg = shared::i2c::Message(kAddress, data1,
                                             shared::i2c::MessageType::Read);

        bindings::i2c_bus1.Read(read_msg);

        std::cout << read_msg << std::endl;

        bindings::DelayMS(1000);
    }

    return 0;
}