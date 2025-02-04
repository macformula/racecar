#include <cstdint>

#include "bindings.h"
#include "shared/comms/i2c/msg.hpp"

int main() {
    bindings::Initialize();

    while (true) {
        uint8_t data1[] = {0x02, 0x03, 0x05};

        bindings::i2c_bus1.Write(
            shared::i2c::Message(0x01, data1, shared::i2c::MessageType::Write));
        bindings::DelayMS(1000);

        uint8_t data2[3];
        auto read_msg =
            shared::i2c::Message(0x03, data2, shared::i2c::MessageType::Read);
        bindings::i2c_bus2.Read(read_msg);
        bindings::DelayMS(1000);
    }

    return 0;
}