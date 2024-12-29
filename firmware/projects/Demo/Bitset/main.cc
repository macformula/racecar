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

        // SampleMessage1 Unpack Test
        uint8_t test_data[] = {0x02, 0xB3, 0x04, 0x05, 0xD4, 0xD4, 0xE6, 0xC2};
        shared::can::RawMessage raw_msg(940, false, 8, test_data);

        // The constructor is usually private. I "hacked" the jinja template to
        // make it public for your testing.
        generated::can::RxSampleMessage1 unpacked_msg(raw_msg, 0);

        assert(unpacked_msg.Val1() == 0xC);
        assert(unpacked_msg.Val2a() == 0x3);
        assert(unpacked_msg.Val2b() == 0x16);
        assert(unpacked_msg.Val3() == 0xC);
        assert(unpacked_msg.Val4() == 0x20);
        assert(unpacked_msg.Val5() == 0x14);
        assert(unpacked_msg.Val6() == 0x26);
        assert(unpacked_msg.Val7() == 0x2E6D);
        assert(unpacked_msg.Val8() == 0x10);

        // SampleMessage2 Unpack Test
        uint8_t test_data2[] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
        shared::can::RawMessage raw_msg2(941, false, 6, test_data2);

        generated::can::RxSampleMessage2 unpacked_msg2(raw_msg2, 0);

        // Notice that 9a and 9b share the same byte. See bitset.dbc
        // 1a is bit 0-3 and 1b is bit7.
        assert(unpacked_msg2.Val9a() == 0x08);
        assert(unpacked_msg2.Val9b() == 0x2E);
        assert(unpacked_msg2.Val10() == 0x2A);
        assert(unpacked_msg2.Val11() == 0x22FCC);
        assert(unpacked_msg2.Val12() == 0x504);

        printf("All unpack tests passed!\n");
    }

    // PACK TEST
    {
        // SampleMessage1 Pack Test
        generated::can::TxSampleMessage1 packed_msg{
            .val1 = 0xC,
            .val2a = 0x3,
            .val2b = 0x16,
            .val3 = 0xC,
            .val4 = 0x20,
            .val5 = 0x14,
            .val6 = 0x26,
            .val7 = 0x2E6D,
            .val8 = 0x10,
        };

        shared::can::RawMessage raw_unpacked_msg = packed_msg.pack();

        assert(raw_unpacked_msg.id == 940);
        assert(raw_unpacked_msg.data[0] == 0x02);
        assert(raw_unpacked_msg.data[1] == 0xB3);
        assert(raw_unpacked_msg.data[2] == 0x04);
        assert(raw_unpacked_msg.data[3] == 0x05);
        assert(raw_unpacked_msg.data[4] == 0xD4);
        assert(raw_unpacked_msg.data[5] == 0xD4);
        assert(raw_unpacked_msg.data[6] == 0xE6);
        assert(raw_unpacked_msg.data[7] == 0x42);

        // SampleMessage2 Pack Test
        generated::can::TxSampleMessage2 packed_msg2{
            .val9a = 0x08,
            .val9b = 0x2E,
            .val10 = 0x2A,
            .val11 = 0x22FCC,
            .val12 = 0x504,
        };

        shared::can::RawMessage raw_unpacked_msg2 = packed_msg2.pack();

        assert(raw_unpacked_msg2.id == 941);
        assert(raw_unpacked_msg2.data[0] == 0xA1);
        assert(raw_unpacked_msg2.data[1] == 0x30);
        assert(raw_unpacked_msg2.data[2] == 0xC0);
        assert(raw_unpacked_msg2.data[3] == 0xD4);
        assert(raw_unpacked_msg2.data[4] == 0xE5);
        assert(raw_unpacked_msg2.data[5] == 0x16);

        printf("All pack tests passed!\n");
    }
    return 0;
}