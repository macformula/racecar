#include "../generated/can/test_rx_bus.hpp"
#include "gtest/gtest.h"
#include "lib/periph/can.hpp"

#define EXPECT_MSG_EQ(x, y)                                      \
    ASSERT_EQ((x).data_length, (y).data_length);                 \
    EXPECT_EQ((x).id, (y).id);                                   \
    EXPECT_EQ((x).is_extended_frame, (y).is_extended_frame);     \
    for (size_t i = 0; i < (x).data_length; ++i) {               \
        EXPECT_EQ((x).data[i], (y).data[i]) << "at index " << i; \
    }

using namespace generated::can;
using macfe::can::RawMessage;

class Bus : public testing::Test {
    class DummyCanBase : public macfe::periph::CanBase {
    public:
        void Send(const macfe::can::RawMessage& m) override {
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
class CanTest : public Bus,
                public testing::WithParamInterface<std::tuple<RawMessage, T>> {
};