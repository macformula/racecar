#include <stdexcept>
#include <tuple>

#include "../generated/can/test_rx_bus.hpp"
#include "../generated/can/test_rx_messages.hpp"
#include "gtest/gtest.h"
#include "projects/Demo/TestCangen/generated/can/test_rx_messages.hpp"
#include "shared/comms/can/msg.hpp"
#include "shared/periph/can.hpp"

using namespace generated::can;
using shared::can::RawMessage;

class Bus : public testing::Test {
    class DummyCanBase : public shared::periph::CanBase {
    public:
        void Send(const shared::can::RawMessage& m) override {
            throw std::runtime_error(
                "Dummy bus is only for receiving, not sending.");
        }

    private:
        uint32_t GetTimestamp() const override {
            return 0;
        }
    };

protected:
    Bus() : base(), bus(base) {}

    DummyCanBase base;
    Test_rxBus bus;
};

template <typename T>
class RxTest : public Bus,
               public testing::WithParamInterface<std::tuple<RawMessage, T>> {};

/***************************************************************
    Booleans
***************************************************************/
struct ExpectedBool {
    bool imd;
    bool bms;
};

using BoolNonAligned = RxTest<ExpectedBool>;
TEST_P(BoolNonAligned, HandlesCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.PopRxBoolNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->IMDLed(), expected.imd);
    EXPECT_EQ(msg->BMSLed(), expected.bms);
}

using BoolAligned = RxTest<ExpectedBool>;
TEST_P(BoolAligned, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.PopRxBoolAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->IMDLed(), expected.imd);
    EXPECT_EQ(msg->BMSLed(), expected.bms);
}

INSTANTIATE_TEST_SUITE_P(
    RxNonAlignedCases, BoolNonAligned,
    // clang-format off
    testing::Values(
    std::tuple{RawMessage{258, false, 2, {0, 0x00}}, ExpectedBool{false, false}},
    std::tuple{RawMessage{258, false, 2, {0, 0x10}}, ExpectedBool{false, true}},
    std::tuple{RawMessage{258, false, 2, {0, 0x08}}, ExpectedBool{true, false}},
    std::tuple{RawMessage{258, false, 2, {0, 0x18}}, ExpectedBool{true, true}}
));
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    RxAlignedCases, BoolAligned,
    // clang-format off
    testing::Values(
    std::tuple{RawMessage{259, false, 4, {0, 0, 0, 0}}, ExpectedBool{false, false}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 1, 0}}, ExpectedBool{true, false}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 0, 1}}, ExpectedBool{false, true}},
    std::tuple{RawMessage{259, false, 4, {0, 0, 1, 1}}, ExpectedBool{true, true}}
));
// clang-format on

/***************************************************************
    Long Fields
***************************************************************/
struct ExpectedLongField {
    uint8_t byte;
    uint16_t halfword;
    uint32_t word;
};

using LongField = RxTest<ExpectedLongField>;
TEST_P(LongField, LongField) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.PopRxLongFields();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->byte(), expected.byte);
    EXPECT_EQ(msg->halfword(), expected.halfword);
    EXPECT_EQ(msg->word(), expected.word);
}

using LongFieldBigEndian = RxTest<ExpectedLongField>;
TEST_P(LongFieldBigEndian, LongField) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.PopRxLongFieldsBigEndian();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->byte(), expected.byte);
    EXPECT_EQ(msg->halfword(), expected.halfword);
    EXPECT_EQ(msg->word(), expected.word);
}

INSTANTIATE_TEST_SUITE_P(
    LongFieldCases, LongField,
    ::testing::Values(std::tuple{
        RawMessage{260, false, 7, {0x08, 0xA2, 0xB3, 0x11, 0x02, 0x03, 0x44}},
        ExpectedLongField{0x08, 0xb3a2, 0x44030211},
    }));

INSTANTIATE_TEST_SUITE_P(
    LongFieldBigEndianCases, LongFieldBigEndian,
    ::testing::Values(std::tuple{
        RawMessage{261, false, 7, {0x08, 0xA2, 0xB3, 0x11, 0x02, 0x03, 0x44}},
        ExpectedLongField{0x08, 0xa2b3, 0x11020344},
    }));

/***************************************************************
    Non-Aligned Signals
***************************************************************/
struct ExpectedNonAligned {
    uint16_t sig1;
    uint8_t sig2;
    uint32_t sig3;
    bool sig4;
    uint8_t sig5;
};

using NonAlignedSignal = RxTest<ExpectedNonAligned>;
TEST_P(NonAlignedSignal, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.PopRxNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), expected.sig1);
    EXPECT_EQ(msg->sig2(), expected.sig2);
    EXPECT_EQ(msg->sig3(), expected.sig3);
    EXPECT_EQ(msg->sig4(), expected.sig4);
    EXPECT_EQ(msg->sig5(), expected.sig5);
}

INSTANTIATE_TEST_SUITE_P(
    NonAlignedSignalCases, NonAlignedSignal,
    // clang-format off
    testing::Values(
    std::tuple{
        RawMessage{262, false, 6, {0xf3, 0x25 | 0x80, 0x16 | 0x50, 0xa3, 0x94, 0x0e | 0x10 | 0xA0}},
        ExpectedNonAligned{0x25f3, 0x1680 >> 6, 0xe94a350 >> 5, true, 5},
    },
    std::tuple{
        RawMessage{262, false, 6, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
        ExpectedNonAligned{0x3fff, 0x7f, 0x7fffff, true, 0x7},
    }
));
// clang-format on

/***************************************************************
    Enumerations
***************************************************************/
struct ExpectedEnum {
    RxEnum::ecu_t ecu;
    RxEnum::state_t state;
};

using Enum = RxTest<ExpectedEnum>;
TEST_P(Enum, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.GetRxEnum();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->ecu(), expected.ecu);
    EXPECT_EQ(msg->state(), expected.state);
}

INSTANTIATE_TEST_SUITE_P(
    EnumCases, Enum,
    // clang-format off
    testing::Values(
    std::tuple{
        RawMessage{263, false, 2, {0x00, 0x00}},
        ExpectedEnum{RxEnum::ecu_t::FrontController, RxEnum::state_t::Waiting},
    },
    std::tuple{
        RawMessage{263, false, 2, {0x01, 0x02}},
        ExpectedEnum{RxEnum::ecu_t::LVController, RxEnum::state_t::Invalid},
    },
    std::tuple{
        RawMessage{263, false, 2, {0x02, 0x01}},
        ExpectedEnum{RxEnum::ecu_t::TMS, RxEnum::state_t::Valid},
    }
));
// clang-format on

/***************************************************************
    Signed Signals
***************************************************************/
struct ExpectedSigned {
    int8_t sig1;
    int16_t sig2;
    int32_t sig3;
};

using SignedLittle = RxTest<ExpectedSigned>;
TEST_P(SignedLittle, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.GetRxSignedLittle();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), expected.sig1);
    EXPECT_EQ(msg->sig2(), expected.sig2);
    EXPECT_EQ(msg->sig3(), expected.sig3);
}

using SignedBig = RxTest<ExpectedSigned>;
TEST_P(SignedBig, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.GetRxSignedBig();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), expected.sig1);
    EXPECT_EQ(msg->sig2(), expected.sig2);
    EXPECT_EQ(msg->sig3(), expected.sig3);
}

INSTANTIATE_TEST_SUITE_P(
    SignedLittleCases, SignedLittle,
    ::testing::Values(
        std::tuple{
            RawMessage{265, false, 5, {0x20 | 0xc0, 0xff, 0x03, 0x00, 0x80}},
            ExpectedSigned{-32, 0x0FFF, -0x100000},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x1f, 0x00, 0xf8, 0xff, 0x7f}},
            ExpectedSigned{31, 0, 0xFFFFF},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x3c | 0x00, 0xff, 0xa5, 0xff, 0xff}},
            ExpectedSigned{-4, -2052, -12},
        },
        std::tuple{
            RawMessage{265, false, 5, {0x1c | 0x00, 0xff, 0xa3, 0xff, 0x5f}},
            ExpectedSigned{28, 4092, 786420},
        }));

INSTANTIATE_TEST_SUITE_P(
    SignedBigCases, SignedBig,
    ::testing::Values(
        std::tuple{
            RawMessage{266, false, 5, {0x80, 0x00, 0x10, 0x00, 0x00}},
            ExpectedSigned{-32, 0, -0x100000},
        },
        std::tuple{
            RawMessage{266, false, 5, {0x7c, 0x00, 0x0f, 0xff, 0xff}},
            ExpectedSigned{31, 0, 0xFFFFF},
        },
        std::tuple{
            RawMessage{266, false, 5, {0xf0 | 0x02, 0xff, 0x9f, 0xff, 0xf4}},
            ExpectedSigned{-4, -2052, -12},
        },
        std::tuple{
            RawMessage{266, false, 5, {0x70 | 0x01, 0xff, 0x8B, 0xff, 0xf4}},
            ExpectedSigned{28, 4092, 786420},
        }));

/***************************************************************
    Floats
***************************************************************/

struct ExpectedFloat {
    float f1;
    float f2;
};

using FloatUnsigned = RxTest<ExpectedFloat>;
TEST_P(FloatUnsigned, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.GetRxFloatUnsigned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_FLOAT_EQ(msg->f1(), expected.f1);
    EXPECT_FLOAT_EQ(msg->f2(), expected.f2);
}

INSTANTIATE_TEST_SUITE_P(FloatUnsignedCases, FloatUnsigned,
                         // clang-format off
    ::testing::Values(
        std::tuple{
            RawMessage{267, false, 3, {0x00, 0xFF, 0xFF}},
            ExpectedFloat{-10, 50 + 65.535},
        },
        std::tuple{
            RawMessage{267, false, 3, {0x01, 0xd2, 0x04}},
            ExpectedFloat{6, 51.234},
        },
        std::tuple{
            RawMessage{267, false, 3, {0xFF, 0x01, 0x00}},
            ExpectedFloat{0xfe6, 50.001},
        },
        std::tuple{
            RawMessage{267, false, 3, {0x0A, 0xA0, 0xB0}},
            ExpectedFloat{150, 50 + 45.216},
        }
    )
);

using FloatSigned = RxTest<ExpectedFloat>;
TEST_P(FloatSigned, HandleCases) {
    auto [raw, expected] = GetParam();

    base.AddToBus(raw);
    auto msg = bus.GetRxFloatSigned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_FLOAT_EQ(msg->f1(), expected.f1);
    EXPECT_FLOAT_EQ(msg->f2(), expected.f2);
}

INSTANTIATE_TEST_SUITE_P(FloatSignedCases, FloatSigned,
    // clang-format off
    ::testing::Values(
        std::tuple{
            RawMessage{268, false, 3, {0x00, 0xFF, 0xFF}},
            ExpectedFloat{-10, 49.999},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x80, 0xd2, 0x04}},
            ExpectedFloat{-2058, 51.234},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x07, 0x01, 0x00}},
            ExpectedFloat{102, 50.001},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x0A, 0x00, 0x80}},
            ExpectedFloat{150, 50 - 32.768},
        }
    )
);
// clang-format on