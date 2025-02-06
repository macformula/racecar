#include <cstdint>

#include "bindings.h"
#include "shared/comms/i2c/msg.hpp"

int main() {
    bindings::Initialize();

    uint8_t data1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    uint8_t data2[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00};

    while (true) {
        auto button_state = bindings::button1.Read();
        if (button_state) {
            bindings::i2c_bus1A.Write(shared::i2c::Message(
                0x01, data1, shared::i2c::MessageType::Write));
            data1[4] += 0x01;
        } else {
            bindings::i2c_bus1B.Write(shared::i2c::Message(
                0x02, data2, shared::i2c::MessageType::Write));
            data2[4] += 0x01;
        }

        bindings::DelayMS(1000);
    }

    return 0;
}