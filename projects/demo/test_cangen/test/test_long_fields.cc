#include "cantest.hpp"
#include "generated/can/test_rx_messages.hpp"
#include "generated/can/test_tx_messages.hpp"

using LongField = CanTest<TxLongFields>;
TEST_P(LongField, LongField) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxLongFields();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->byte(), tx.byte);
    EXPECT_EQ(msg->halfword(), tx.halfword);
    EXPECT_EQ(msg->word(), tx.word);
}

INSTANTIATE_TEST_SUITE_P(
    LongFieldCases, LongField,
    ::testing::Values(std::tuple{
        RawMessage{260, false, 7, {0x08, 0xA2, 0xB3, 0x11, 0x02, 0x03, 0x44}},
        TxLongFields{0x08, 0xb3a2, 0x44030211},
    }));

using LongFieldBigEndian = CanTest<TxLongFieldsBigEndian>;
TEST_P(LongFieldBigEndian, LongField) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxLongFieldsBigEndian();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->byte(), tx.byte);
    EXPECT_EQ(msg->halfword(), tx.halfword);
    EXPECT_EQ(msg->word(), tx.word);
}

INSTANTIATE_TEST_SUITE_P(
    LongFieldBigEndianCases, LongFieldBigEndian,
    ::testing::Values(
        std::tuple{
            RawMessage{
                261, false, 7, {0x08, 0xA2, 0xB3, 0x11, 0x02, 0x03, 0x44}},
            TxLongFieldsBigEndian{0x08, 0xa2b3, 0x11020344},
        },
        std::tuple{
            RawMessage{
                261, false, 7, {0xda, 0xdb, 0x54, 0x7a, 0x6b, 0x5c, 0x4d}},
            TxLongFieldsBigEndian{0xda, 0xdb54, 0x7a6b5c4d},
        }));