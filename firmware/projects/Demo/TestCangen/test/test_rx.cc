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

TEST_F(Bus, RxNonAligned) {
    {
        uint8_t data[8]{0, 0x00, 0, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{258, true, 4, data});
        auto msg = bus.PopRxStatusNonAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), false);
        ASSERT_EQ(msg->BMSLed(), false);
    }
    {
        uint8_t data[8]{0, 0x10, 0, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{258, true, 4, data});
        auto msg = bus.PopRxStatusNonAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), false);
        ASSERT_EQ(msg->BMSLed(), true);
    }
    {
        uint8_t data[8]{0, 0x04, 0, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{258, true, 4, data});
        auto msg = bus.PopRxStatusNonAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), true);
        ASSERT_EQ(msg->BMSLed(), false);
    }
    {
        uint8_t data[8]{0, 0x14, 0, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{258, true, 4, data});
        auto msg = bus.PopRxStatusNonAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), true);
        ASSERT_EQ(msg->BMSLed(), true);
    }
}

TEST_F(Bus, RxAligned) {
    {
        uint8_t data[8]{0, 0, 0, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{259, true, 4, data});
        auto msg = bus.PopRxStatusAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), false);
        ASSERT_EQ(msg->BMSLed(), false);
    }
    {
        uint8_t data[8]{0, 0, 0, 1, 0, 0, 0, 0};
        base.AddToBus(RawMessage{259, true, 4, data});
        auto msg = bus.PopRxStatusAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), false);
        ASSERT_EQ(msg->BMSLed(), true);
    }
    {
        uint8_t data[8]{0, 0, 1, 0, 0, 0, 0, 0};
        base.AddToBus(RawMessage{259, true, 4, data});
        auto msg = bus.PopRxStatusAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), true);
        ASSERT_EQ(msg->BMSLed(), false);
    }
    {
        uint8_t data[8]{0, 0, 1, 1, 0, 0, 0, 0};
        base.AddToBus(RawMessage{259, true, 4, data});
        auto msg = bus.PopRxStatusAligned();
        ASSERT_TRUE(msg.has_value());
        ASSERT_EQ(msg->IMDLed(), true);
        ASSERT_EQ(msg->BMSLed(), true);
    }
}