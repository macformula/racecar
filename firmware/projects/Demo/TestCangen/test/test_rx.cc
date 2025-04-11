#include <stdexcept>

#include "../generated/can/test_bus.hpp"
#include "../generated/can/test_messages.hpp"
#include "gtest/gtest.h"
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
    TestBus bus;
};

struct StatusTestCase {
    uint8_t data[8];
    bool expected_imd;
    bool expected_bms;
};

class RxNonAlignedStatus : public Bus,
                           public testing::WithParamInterface<StatusTestCase> {
};

TEST_P(RxNonAlignedStatus, HandlesCases) {
    auto [data, expected_imd, expected_bms] = GetParam();

    base.AddToBus(RawMessage{258, false, 2, data});
    auto msg = bus.PopRxStatusNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->IMDLed(), expected_imd);
    EXPECT_EQ(msg->BMSLed(), expected_bms);
}

INSTANTIATE_TEST_SUITE_P(
    RxNonAlignedCases, RxNonAlignedStatus,
    testing::Values(StatusTestCase{{0, 0x00}, false, false},
                    StatusTestCase{{0, 0x10}, false, true},
                    StatusTestCase{{0, 0x08}, true, false},
                    StatusTestCase{{0, 0x18}, true, true}));

class RxAlignedTest : public Bus,
                      public testing::WithParamInterface<StatusTestCase> {};

TEST_P(RxAlignedTest, HandleCases) {
    auto [data, expected_imd, expected_bms] = GetParam();

    base.AddToBus(RawMessage{259, false, 4, data});
    auto msg = bus.PopRxStatusAligned();

    ASSERT_TRUE(msg.has_value());
    ASSERT_EQ(msg->IMDLed(), expected_imd);
    ASSERT_EQ(msg->BMSLed(), expected_bms);
}

INSTANTIATE_TEST_SUITE_P(
    RxAlignedCases, RxAlignedTest,
    testing::Values(StatusTestCase{{0, 0, 0, 0}, false, false},
                    StatusTestCase{{0, 0, 1, 0}, true, false},
                    StatusTestCase{{0, 0, 0, 1}, false, true},
                    StatusTestCase{{0, 0, 1, 1}, true, true}));

TEST_F(Bus, LongField) {
    uint8_t data[8] = {0x08, 0xA2, 0xB3, 0x11, 0x02, 0x03, 0x44};
    {  // Little Endian
        base.AddToBus(RawMessage{260, false, 7, data});
        auto msg = bus.PopRxLongFields();

        ASSERT_TRUE(msg.has_value());
        EXPECT_EQ(msg->byte(), 0x08);
        EXPECT_EQ(msg->halfword(), 0xB3A2);
        EXPECT_EQ(msg->word(), 0x44030211);
    }
    {  // Big Endian
        base.AddToBus(RawMessage{261, false, 7, data});
        auto msg = bus.PopRxLongFieldsBigEndian();

        ASSERT_TRUE(msg.has_value());
        EXPECT_EQ(msg->byte(), 0x08);
        EXPECT_EQ(msg->halfword(), 0xA2B3);
        EXPECT_EQ(msg->word(), 0x11020344);
    }
}

struct NonAlignedCase {
    uint8_t data[6];
    uint16_t exp1;
    uint8_t exp2;
    uint32_t exp3;
    bool exp4;
    uint8_t exp5;
};

class RxNonAlignedSignalTest
    : public Bus,
      public testing::WithParamInterface<NonAlignedCase> {};

TEST_P(RxNonAlignedSignalTest, HandleCases) {
    auto p = GetParam();

    base.AddToBus(RawMessage{262, false, 6, p.data});
    auto msg = bus.PopRxNonAligned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->sig1(), p.exp1);
    EXPECT_EQ(msg->sig2(), p.exp2);
    EXPECT_EQ(msg->sig3(), p.exp3);
    EXPECT_EQ(msg->sig4(), p.exp4);
    EXPECT_EQ(msg->sig5(), p.exp5);
}

INSTANTIATE_TEST_SUITE_P(
    RxNonAlignedSignalCases, RxNonAlignedSignalTest,
    testing::Values(NonAlignedCase{{0xf3, 0x25 | 0x80, 0x16 | 0x50, 0xa3, 0x94,
                                    0x0e | 0x10 | 0xA0},
                                   0x25f3,
                                   0x1680 >> 6,
                                   0xe94a350 >> 5,
                                   true,
                                   5},
                    NonAlignedCase{{0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
                                   0x3fff,
                                   0x7f,
                                   0x7fffff,
                                   true,
                                   0x7}));