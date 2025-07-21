#include "../generated/can/test_rx_messages.hpp"
#include "../generated/can/test_tx_messages.hpp"
#include "cantest.hpp"

using BoolNonAligned = CanTest<TxBoolNonAligned>;
TEST_P(BoolNonAligned, HandlesCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxBoolNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->IMDLed(), tx.imd_led);
    EXPECT_EQ(msg->BMSLed(), tx.bms_led);
}

INSTANTIATE_TEST_SUITE_P(
    RxNonAlignedCases, BoolNonAligned,
    // clang-format off
    testing::Values(
    std::tuple{RawMessage{258, false, 2, {0, 0x00}}, TxBoolNonAligned{false, false}},
    std::tuple{RawMessage{258, false, 2, {0, 0x10}}, TxBoolNonAligned{false, true}},
    std::tuple{RawMessage{258, false, 2, {0, 0x08}}, TxBoolNonAligned{true, false}},
    std::tuple{RawMessage{258, false, 2, {0, 0x18}}, TxBoolNonAligned{true, true}}
));
// clang-format on

using BoolAligned = CanTest<TxBoolAligned>;
TEST_P(BoolAligned, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.PopRxBoolAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->IMDLed(), tx.imd_led);
    EXPECT_EQ(msg->BMSLed(), tx.bms_led);
}

INSTANTIATE_TEST_SUITE_P(
    RxAlignedCases, BoolAligned,
    // clang-format off
    testing::Values(
    std::tuple{RawMessage{259, false, 4, {0, 0, 0, 0}}, TxBoolAligned{false, false}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 1, 0}}, TxBoolAligned{true, false}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 0, 1}}, TxBoolAligned{false, true}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 1, 1}}, TxBoolAligned{true, true}}
));
// clang-format on