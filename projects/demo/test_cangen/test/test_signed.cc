#include "cantest.hpp"
#include "generated/can/test_rx_messages.hpp"
#include "generated/can/test_tx_messages.hpp"

using SignedLittle = CanTest<TxSignedLittle>;
TEST_P(SignedLittle, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.GetRxSignedLittle();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), tx.sig1);
    EXPECT_EQ(msg->sig2(), tx.sig2);
    EXPECT_EQ(msg->sig3(), tx.sig3);
}

INSTANTIATE_TEST_SUITE_P(
    SignedLittleCases, SignedLittle,
    ::testing::Values(
        std::tuple{
            RawMessage{265, false, 5, {0x20 | 0xc0, 0xff, 0x03, 0x00, 0x80}},
            TxSignedLittle{-32, 0x0FFF, -0x100000},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x1f, 0x00, 0xf8, 0xff, 0x7f}},
            TxSignedLittle{31, 0, 0xFFFFF},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x3c | 0x00, 0xff, 0xa5, 0xff, 0xff}},
            TxSignedLittle{-4, -2052, -12},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x1c | 0x00, 0xff, 0xa3, 0xff, 0x5f}},
            TxSignedLittle{28, 4092, 786420},
        }));

using SignedBig = CanTest<TxSignedBig>;
TEST_P(SignedBig, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.GetRxSignedBig();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), tx.sig1);
    EXPECT_EQ(msg->sig2(), tx.sig2);
    EXPECT_EQ(msg->sig3(), tx.sig3);
}

INSTANTIATE_TEST_SUITE_P(
    SignedBigCases, SignedBig,
    ::testing::Values(
        std::tuple{
            RawMessage{266, false, 5, {0x80, 0x00, 0x10, 0x00, 0x00}},
            TxSignedBig{-32, 0, -0x100000},
        },
        std::tuple{
            RawMessage{266, false, 5, {0x7c, 0x00, 0x0f, 0xff, 0xff}},
            TxSignedBig{31, 0, 0xFFFFF},
        },
        std::tuple{
            RawMessage{266, false, 5, {0xf0 | 0x02, 0xff, 0x9f, 0xff, 0xf4}},
            TxSignedBig{-4, -2052, -12},
        },
        std::tuple{
            RawMessage{266, false, 5, {0x70 | 0x01, 0xff, 0x8B, 0xff, 0xf4}},
            TxSignedBig{28, 4092, 786420},
        }));
