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

        uint8_t test_data[] = {0xA1, 0x02, 0xB3, 0x04, 0x05};
        shared::can::RawMessage raw_msg(940, false, 5, test_data);

        // The constructor is usually private. I "hacked" the jinja template to
        // make it public for your testing.
        generated::can::RxSampleMessage unpacked_msg(raw_msg, 0);

        // Notice that 1a and 1b share the same byte. See bitset.dbc
        // 1a is bit 0-3 and 1b is bit7.
        assert(unpacked_msg.Val1a() == 0x1);
        assert(unpacked_msg.Val1b() == 0xA);
        assert(unpacked_msg.Val2() == 0xC);
        assert(unpacked_msg.Val3a() == 0x3);
        assert(unpacked_msg.Val3b() == 0x16);
        assert(unpacked_msg.Val4() == 0xC);
        assert(unpacked_msg.Val5() == 0x20);

        // uint8_t test_data2[] = {0x01, 0x02, 0x03, 0x04};
        // shared::can::RawMessage raw_msg2(941, false, 4, test_data2);

        // // The constructor is usually private. I "hacked" the jinja template
        // to
        // // make it public for your testing.
        // generated::can::RxSampleMessage unpacked_msg2(raw_msg2, 0);

        // // Notice that 1a and 1b share the same byte. See bitset.dbc
        // // 1a is bit 0-3 and 1b is bit7.
        // assert(unpacked_msg2.Val1c() == 0x1);
        // assert(unpacked_msg2.Val2c() == 0x2);
        // assert(unpacked_msg2.Val3c() == 0x3);
        // assert(unpacked_msg2.Val4c() == 0x4);

        // uint8_t test_data[] = {0xA4, 0xA5, 0xA6, 0xA7, 0x08, 0x09, 0x0A,
        // 0x0B}; shared::can::RawMessage raw_msg(645, false, 11, test_data);

        // // The constructor is usually private. I "hacked" the jinja template
        // to
        // // make it public for your testing.
        // generated::can::RxSampleMessage unpacked_msg(raw_msg, 0);

        // // Notice that 1a and 1b share the same byte. See bitset.dbc
        // // 1a is bit 0-3 and 1b is bit7.
        // assert(unpacked_msg.AMK_MagnetizingCurrent() == 0x1);
        // assert(unpacked_msg.AMK_TorqueCurrent() == 0xA);
        // assert(unpacked_msg.AMK_ActualVelocity() == 0xC);
        // assert(unpacked_msg.AMK_bDerating() == 0x1);    // done
        // assert(unpacked_msg.AMK_bInverterOn() == 0x0);  // done
        // assert(unpacked_msg.AMK_bQuitInverterOn() == 0x1);  // done
        // assert(unpacked_msg.AMK_bDcOn() == 0x0);    // done
        // assert(unpacked_msg.AMK_bQuitDcOn() == 0x1);    // done
        // assert(unpacked_msg.AMK_bWarn() == 0x0);    // done
        // assert(unpacked_msg.AMK_bError() == 0x1);   // done
        // assert(unpacked_msg.AMK_bSystemReady() == 0x1);     // done

        printf("All unpack tests passed!\n");
    }

    // PACK TEST
    {
        generated::can::TxSampleMessage packed_msg{
            .val1a = 0x1,
            .val1b = 0xA,
            .val2 = 0x2,
            .val3a = 0x3,
            .val3b = 0xB,
            .val4 = 0x4,
            .val5 = 0x5,
        };

        shared::can::RawMessage raw_unpacked_msg = packed_msg.pack();

        assert(raw_unpacked_msg.id == 940);
        assert(raw_unpacked_msg.data[0] == 0xA1);
        assert(raw_unpacked_msg.data[1] == 0xF8);
        assert(raw_unpacked_msg.data[2] == 0x5B);
        assert(raw_unpacked_msg.data[3] == 0x01);
        assert(raw_unpacked_msg.data[4] == 0x00);

        // generated::can::TxSampleMessage packed_msg2{
        //     .val1c = 0x1,
        //     .val2c = 0x2,
        //     .val3c = 0x3,
        //     .val4c = 0x4,
        // };

        // shared::can::RawMessage raw_unpacked_msg2 = packed_msg2.pack();

        // assert(raw_unpacked_msg2.id == 941);
        // assert(raw_unpacked_msg2.data[0] == 0x01);
        // assert(raw_unpacked_msg2.data[1] == 0x02);
        // assert(raw_unpacked_msg2.data[2] == 0x03);
        // assert(raw_unpacked_msg2.data[3] == 0x04);

        printf("All pack tests passed!\n");
    }
    return 0;
}