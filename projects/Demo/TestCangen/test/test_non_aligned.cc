#include "../generated/can/test_rx_messages.hpp"
#include "../generated/can/test_tx_messages.hpp"
#include "cantest.hpp"

using NonAlignedSignal = CanTest<TxNonAligned>;
TEST_P(NonAlignedSignal, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), tx.sig1);
    EXPECT_EQ(msg->sig2(), tx.sig2);
    EXPECT_EQ(msg->sig3(), tx.sig3);
    EXPECT_EQ(msg->sig4(), tx.sig4);
    EXPECT_EQ(msg->sig5(), tx.sig5);
}

INSTANTIATE_TEST_SUITE_P(
    NonAlignedSignalCases, NonAlignedSignal,
    // clang-format off
    testing::Values(
    std::tuple{
        RawMessage{262, false, 6, {0xf3, 0x25 | 0x80, 0x16 | 0x50, 0xa3, 0x94, 0x0e | 0x10 | 0xA0}},
        TxNonAligned{0x25f3, 0x1680 >> 6, 0xe94a350 >> 5, true, 5},
    },
    std::tuple{
        RawMessage{262, false, 6, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
        TxNonAligned{0x3fff, 0x7f, 0x7fffff, true, 0x7},
    }
));
// clang-format on

using NonAlignedBigEndian = CanTest<TxNonAlignedBigEndian>;
TEST_P(NonAlignedBigEndian, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxNonAlignedBigEndian();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), tx.sig1);
    EXPECT_EQ(msg->sig2(), tx.sig2);
    EXPECT_EQ(msg->sig3(), tx.sig3);
    EXPECT_EQ(msg->sig4(), tx.sig4);
    EXPECT_EQ(msg->sig5(), tx.sig5);
}

INSTANTIATE_TEST_SUITE_P(
    NonAlignedEndianCases, NonAlignedBigEndian,
    // clang-format off
    ::testing::Values(
        std::tuple{
            RawMessage{264, false, 6, {0x97, 0xcc | 0x02, 0xd0 | 0x07, 0x4a, 0x51, 0xB0 | 0x08 | 0x05}},
            TxNonAlignedBigEndian{0x25f3, 0x5A, 0x74a51b, true, 5},
        }
    )
);
// clang-format on
