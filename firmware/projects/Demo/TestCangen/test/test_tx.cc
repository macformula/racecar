#include "../generated/can/test_tx_messages.hpp"
#include "gtest/gtest.h"
#include "shared/comms/can/msg.hpp"

#define EXPECT_MSG_EQ(x, y)                                      \
    ASSERT_EQ((x).data_length, (y).data_length);                 \
    EXPECT_EQ((x).id, (y).id);                                   \
    EXPECT_EQ((x).is_extended_frame, (y).is_extended_frame);     \
    for (size_t i = 0; i < (x).data_length; ++i) {               \
        EXPECT_EQ((x).data[i], (y).data[i]) << "at index " << i; \
    }

using namespace generated::can;
using shared::can::RawMessage;

#include <variant>

// When testing a new message, add it to the list of variants
struct Case {
    std::variant<TxStatusAligned, TxStatusNonAligned, TxEnum, TxNonAligned> tx;
    RawMessage expected;
};

class TxTest : public ::testing::TestWithParam<Case> {};

TEST_P(TxTest, StatusTest) {
    const auto& test_case = GetParam();
    const auto packed_msg =
        std::visit([](const auto& tx) { return tx.pack(); }, test_case.tx);
    EXPECT_MSG_EQ(packed_msg, test_case.expected);
}

INSTANTIATE_TEST_SUITE_P(
    TxStatusAligned, TxTest,
    ::testing::Values(Case{TxStatusAligned{false, false},
                           RawMessage{259, false, 4, {0, 0, 0, 0}}},
                      Case{
                          TxStatusAligned{false, true},
                          RawMessage{259, false, 4, {0, 0, 0, 1}},
                      },
                      Case{
                          TxStatusAligned{true, false},
                          RawMessage{259, false, 4, {0, 0, 1, 0}},
                      },
                      Case{
                          TxStatusAligned{true, true},
                          RawMessage{259, false, 4, {0, 0, 1, 1}},
                      }));

INSTANTIATE_TEST_SUITE_P(TxStatusNonAligned, TxTest,
                         ::testing::Values(
                             Case{
                                 TxStatusNonAligned{false, false},
                                 RawMessage{258, false, 2, {0x00, 0x00}},
                             },
                             Case{
                                 TxStatusNonAligned{true, false},
                                 RawMessage{258, false, 2, {0x00, 0x08}},
                             },
                             Case{
                                 TxStatusNonAligned{false, true},
                                 RawMessage{258, false, 2, {0x00, 0x10}},
                             },
                             Case{
                                 TxStatusNonAligned{true, true},
                                 RawMessage{258, false, 2, {0x00, 0x18}},
                             }));

INSTANTIATE_TEST_SUITE_P(
    TxEnum, TxTest,
    ::testing::Values(
        Case{
            TxEnum{TxEnum::ecu_t::FrontController, TxEnum::state_t::Waiting},
            RawMessage{263, false, 2, {0x00, 0x00}},
        },
        Case{
            TxEnum{TxEnum::ecu_t::FrontController, TxEnum::state_t::Valid},
            RawMessage{263, false, 2, {0x00, 0x01}},
        },
        Case{
            TxEnum{TxEnum::ecu_t::TMS, TxEnum::state_t::Invalid},
            RawMessage{263, false, 2, {0x02, 0x02}},
        },
        Case{
            TxEnum{TxEnum::ecu_t::TMS, TxEnum::state_t::Waiting},
            RawMessage{263, false, 2, {0x02, 0x00}},
        },
        Case{
            TxEnum{TxEnum::ecu_t::LVController, TxEnum::state_t::Valid},
            RawMessage{263, false, 2, {0x01, 0x01}},
        },
        Case{
            TxEnum{TxEnum::ecu_t::LVController, TxEnum::state_t::Invalid},
            RawMessage{263, false, 2, {0x01, 0x02}},
        }));

INSTANTIATE_TEST_SUITE_P(
    TxNonAlignedCase, TxTest,
    ::testing::Values(
        Case{
            TxNonAligned{0x25f3, 0x1680 >> 6, 0xe94a350 >> 5, true, 5},
            RawMessage{262,
                       false,
                       6,
                       {0xf3, 0x25 | 0x80, 0x16 | 0x50, 0xa3, 0x94,
                        0x0e | 0x10 | 0xA0}},
        },
        Case{
            TxNonAligned{0x3fff, 0x7f, 0x7fffff, true, 0x7},
            RawMessage{262, false, 6, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
        }));
