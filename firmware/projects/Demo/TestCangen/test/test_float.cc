#include "../generated/can/test_rx_messages.hpp"
#include "../generated/can/test_tx_messages.hpp"
#include "cantest.hpp"

using FloatUnsigned = CanTest<TxFloatUnsigned>;
TEST_P(FloatUnsigned, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.GetRxFloatUnsigned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_FLOAT_EQ(msg->f1(), tx.f1);
    EXPECT_FLOAT_EQ(msg->f2(), tx.f2);
}

INSTANTIATE_TEST_SUITE_P(FloatUnsignedCases, FloatUnsigned,
                         // clang-format off
    ::testing::Values(
        std::tuple{
            RawMessage{267, false, 3, {0x00, 0xFF, 0xFF}},
            TxFloatUnsigned{-10, 50 + 65.535},
        },
        std::tuple{
            RawMessage{267, false, 3, {0x01, 0xd2, 0x04}},
            TxFloatUnsigned{6, 51.234},
        },
        std::tuple{
            RawMessage{267, false, 3, {0xFF, 0x01, 0x00}},
            TxFloatUnsigned{0xfe6, 50.001},
        },
        std::tuple{
            RawMessage{267, false, 3, {0x0A, 0xA0, 0xB0}},
            TxFloatUnsigned{150, 50 + 45.216},
        }
    )
);

using FloatSigned = CanTest<TxFloatSigned>;
TEST_P(FloatSigned, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.GetRxFloatSigned();

    ASSERT_TRUE(msg.has_value());
    EXPECT_FLOAT_EQ(msg->f1(), tx.f1);
    EXPECT_FLOAT_EQ(msg->f2(), tx.f2);
}

INSTANTIATE_TEST_SUITE_P(FloatSignedCases, FloatSigned,
    // clang-format off
    ::testing::Values(
        std::tuple{
            RawMessage{268, false, 3, {0x00, 0xFF, 0xFF}},
            TxFloatSigned{-10, 49.999},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x80, 0xd2, 0x04}},
            TxFloatSigned{-2058, 51.234},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x07, 0x01, 0x00}},
            TxFloatSigned{102, 50.001},
        },
        std::tuple{
            RawMessage{268, false, 3, {0x0A, 0x00, 0x80}},
            TxFloatSigned{150, 50 - 32.768},
        }
    )
);
// clang-format on