#include <cassert>

#include "generated/can/demobus_messages.hpp"
#include "shared/comms/can/msg.hpp"

using namespace generated::can;

int main(void) {
    // Tamara - there's no while() loop here. We're not interested in a
    // continuously running program but just something which verifies the CAN
    // packing and unpacking.

    // UNPACK TEST
    {  // these braces limit the scope of the variables. It lets us reuse the
       // same variable names later

        uint8_t test_data[] = {0xA1, 0x02, 0x03, 0x04};
        shared::can::RawMessage raw_msg(940, false, 4, test_data);

        // The constructor is usually private. I "hacked" the jinja template to
        // make it public for your testing.
        generated::can::RxSampleMessage unpacked_msg(raw_msg, 0);

        // Notice that 1a and 1b share the same byte. See bitset.dbc
        // 1a is bit 0-3 and 1b is bit7.
        assert(unpacked_msg.Val1a() == 0x1);
        assert(unpacked_msg.Val1b() == 0xA);
        assert(unpacked_msg.Val2() == 0x2);
        assert(unpacked_msg.Val3() == 0x3);
        assert(unpacked_msg.Val4() == 0x4);

        printf("All unpack tests passed!\n");
    }

    // PACK TEST
    {
        generated::can::TxSampleMessage packed_msg{
            .val1a = 0x1,
            .val1b = 0xA,
            .val2 = 0x2,
            .val3 = 0x3,
            .val4 = 0x4,
        };

        shared::can::RawMessage raw_unpacked_msg = packed_msg.pack();

        assert(raw_unpacked_msg.id == 940);
        assert(raw_unpacked_msg.data[0] == 0xA1);
        assert(raw_unpacked_msg.data[1] == 0x02);
        assert(raw_unpacked_msg.data[2] == 0x03);
        assert(raw_unpacked_msg.data[3] == 0x04);

        printf("All pack tests passed!\n");
    }
    return 0;
}