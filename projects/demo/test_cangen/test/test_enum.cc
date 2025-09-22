#include "cantest.hpp"
#include "generated/can/test_rx_messages.hpp"
#include "generated/can/test_tx_messages.hpp"

using Enum = CanTest<TxEnum>;
TEST_P(Enum, HandleCases) {
    auto [raw, tx] = GetParam();

    // Pack
    EXPECT_MSG_EQ(tx.pack(), raw);

    // Unpack
    base.AddToBus(raw);
    auto msg = bus.GetRxEnum();

    ASSERT_TRUE(msg.has_value());

    // The Rx and Tx messages define their identical, but separate, enum
    // classes, so we must cast to compare.
    // This would never occur in firmware since no ECU sends a message to
    // itself.
    EXPECT_EQ(static_cast<TxEnum::ecu_t>(msg->ecu()), tx.ecu);
    EXPECT_EQ(static_cast<TxEnum::state_t>(msg->state()), tx.state);
}

INSTANTIATE_TEST_SUITE_P(
    EnumCases, Enum,
    // clang-format off
    testing::Values(
        std::tuple{
            RawMessage{263, false, 2, {0x00, 0x00}},
            TxEnum{TxEnum::ecu_t::FrontController, TxEnum::state_t::Waiting},
        },
        std::tuple{
            RawMessage{263, false, 2, {0x00, 0x01}},
            TxEnum{TxEnum::ecu_t::FrontController, TxEnum::state_t::Valid},
        },
        std::tuple{
            RawMessage{263, false, 2, {0x02, 0x02}},
            TxEnum{TxEnum::ecu_t::TMS, TxEnum::state_t::Invalid},
        },
        std::tuple{
            RawMessage{263, false, 2, {0x02, 0x00}},
            TxEnum{TxEnum::ecu_t::TMS, TxEnum::state_t::Waiting},
        },
        std::tuple{
            RawMessage{263, false, 2, {0x01, 0x01}},
            TxEnum{TxEnum::ecu_t::LVController, TxEnum::state_t::Valid},
        },
        std::tuple{
            RawMessage{263, false, 2, {0x01, 0x02}},
            TxEnum{TxEnum::ecu_t::LVController, TxEnum::state_t::Invalid},
        }
    )
);
// clang-format on
