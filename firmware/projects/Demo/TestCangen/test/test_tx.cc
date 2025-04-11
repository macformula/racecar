#include <variant>

#include "../generated/can/test_tx_messages.hpp"
#include "gtest/gtest.h"
#include "projects/Demo/TestCangen/generated/can/test_tx_messages.hpp"
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

// When testing a new message, add it to the list of variants
struct Case {
    std::variant<TxBoolAligned, TxBoolNonAligned, TxEnum, TxNonAligned,
                 TxLongFieldsBigEndian, TxNonAlignedBigEndian, TxSignedLittle,
                 TxSignedBig, TxFloatUnsigned, TxFloatSigned>
        tx;
    RawMessage expected;
};

class TxTest : public ::testing::TestWithParam<Case> {};

TEST_P(TxTest, HandleCase) {
    const auto& test_case = GetParam();
    const auto packed_msg =
        std::visit([](const auto& tx) { return tx.pack(); }, test_case.tx);
    EXPECT_MSG_EQ(packed_msg, test_case.expected);
}

/***************************************************************
    Test Cases
***************************************************************/

INSTANTIATE_TEST_SUITE_P(
    BoolAligned, TxTest,
    // clang-format off
    ::testing::Values(
        Case{TxBoolAligned{false, false}, RawMessage{259, false, 4, {0, 0, 0, 0}}},
        Case{TxBoolAligned{false, true},  RawMessage{259, false, 4, {0, 0, 0, 1}}},
        Case{TxBoolAligned{true, false},  RawMessage{259, false, 4, {0, 0, 1, 0}}},
        Case{TxBoolAligned{true, true},   RawMessage{259, false, 4, {0, 0, 1, 1}}}
    )
);
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    BoolNonAligned, TxTest,
    // clang-format off
    ::testing::Values(
        Case{TxBoolNonAligned{false, false}, RawMessage{258, false, 2, {0x00, 0x00}}},
        Case{TxBoolNonAligned{true, false},  RawMessage{258, false, 2, {0x00, 0x08}}},
        Case{TxBoolNonAligned{false, true},  RawMessage{258, false, 2, {0x00, 0x10}}},
        Case{TxBoolNonAligned{true, true},   RawMessage{258, false, 2, {0x00, 0x18}}}
    )
);
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    Enum, TxTest,
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
    NonAlignedCase, TxTest,
    // clang-format off
    ::testing::Values(
        Case{
            TxNonAligned{0x25f3, 0x1680 >> 6, 0xe94a360 >> 5, true, 5},
            RawMessage{262, false, 6, {0xf3, 0x25 | 0x80, 0x16 | 0x60, 0xa3, 0x94, 0x0e | 0x10 | 0xA0}}
        },
        Case{
            TxNonAligned{0x3fff, 0x7f, 0x7fffff, true, 0x7},
            RawMessage{262, false, 6, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
        }
    )
);
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    LongFieldsBigEndianCase, TxTest,
    ::testing::Values(Case{
        TxLongFieldsBigEndian{0xda, 0xdb54, 0x7a6b5c4d},
        RawMessage{261, false, 7, {0xda, 0xdb, 0x54, 0x7a, 0x6b, 0x5c, 0x4d}},
    }));

INSTANTIATE_TEST_SUITE_P(
    NonAlignedEndianCase, TxTest,
    // clang-format off
    ::testing::Values(
        Case{
            TxNonAlignedBigEndian{0x25f3, 0x5A, 0x74a51b, true, 5},
            RawMessage{264, false, 6, {0x97, 0xcc | 0x02, 0xd0 | 0x07, 0x4a, 0x51, 0xB0 | 0x08 | 0x05}},
        }
    )
);
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    SignedLittleCase, TxTest,
    ::testing::Values(
        Case{
            TxSignedLittle{-4, -2052, -12},
            RawMessage{265, false, 5, {0x3c | 0x00, 0xff, 0xa5, 0xff, 0xff}},
        },
        Case{
            TxSignedLittle{28, 4092, 786420},
            RawMessage{265, false, 5, {0x1c | 0x00, 0xff, 0xa3, 0xff, 0x5f}},
        }));

INSTANTIATE_TEST_SUITE_P(
    SignedBigCase, TxTest,
    ::testing::Values(
        Case{
            TxSignedBig{-4, -2052, -12},
            RawMessage{266, false, 5, {0xf0 | 0x02, 0xff, 0x9f, 0xff, 0xf4}},
        },
        Case{
            TxSignedBig{28, 4092, 786420},
            RawMessage{266, false, 5, {0x70 | 0x01, 0xff, 0x8B, 0xff, 0xf4}},
        }));

/***************************************************************
    Floats
***************************************************************/
INSTANTIATE_TEST_SUITE_P(FloatUnsigned, TxTest,
                         // clang-format off
    ::testing::Values(
        Case{
            TxFloatUnsigned{-10, 50 + 65.535},
            RawMessage{267, false, 3, {0x00, 0xFF, 0xFF}},
        },
        Case{
            TxFloatUnsigned{6, 51.234},
            RawMessage{267, false, 3, {0x01, 0xd2, 0x04}},
        },
        Case{
            TxFloatUnsigned{0xfe6, 50.001},
            RawMessage{267, false, 3, {0xFF, 0x01, 0x00}},
        },
        Case{
            TxFloatUnsigned{150, 50 + 45.216},
            RawMessage{267, false, 3, {0x0A, 0xA0, 0xB0}},
        }
    )
);
// clang-format on

INSTANTIATE_TEST_SUITE_P(FloatSigned, TxTest,
                         // clang-format off
    ::testing::Values(
        Case{
            TxFloatSigned{-10, 49.999},
            RawMessage{268, false, 3, {0x00, 0xFF, 0xFF}},
        },
        Case{
            TxFloatSigned{-2058, 51.234},
            RawMessage{268, false, 3, {0x80, 0xd2, 0x04}},
        },
        Case{
            TxFloatSigned{102, 50.001},
            RawMessage{268, false, 3, {0x07, 0x01, 0x00}},
        },
        Case{
            TxFloatSigned{150, 50 - 32.768},
            RawMessage{268, false, 3, {0x0A, 0x00, 0x80}},
        }  
    )
);
// clang-format on