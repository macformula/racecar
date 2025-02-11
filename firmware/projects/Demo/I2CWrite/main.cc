#include <cstdint>
#include <iostream>

#include "bindings.hpp"
#include "shared/comms/i2c/msg.hpp"

int main() {
    bindings::Initialize();

    constexpr uint8_t kAddress = 0x2C;

    while (true) {
        uint8_t data1[] = {0xDE, 0xAD, 0xBE, 0xEF};
        auto write_msg = shared::i2c::Message(kAddress, data1,
                                              shared::i2c::MessageType::Write);
        std::cout << "WR -> " << write_msg << std::endl;

        bindings::i2c_bus1.Write(write_msg);
        bindings::DelayMS(1000);
    }

    return 0;
}