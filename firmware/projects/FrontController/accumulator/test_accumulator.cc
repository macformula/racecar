#include "../enums.hpp"
#include "accumulator.hpp"
#include "gtest/gtest.h"

// this is not a test itself, rather it produces a BatteryMonitor object in a
// specific state to make it easier to run other tests
Accumulator CycleToState(AccSts desired_state, int& time_ms) {
    using enum ContactorFeedback::State;

    Accumulator bm{};
    time_ms = 0;

    // Initialize the BatteryMonitor
    auto output1 = bm.Update(
        {
            .cmd = AccCmd::INIT,
            .feedback{
                .precharge = IS_OPEN,
                .negative = IS_OPEN,
                .positive = IS_OPEN,
            },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output1.status == AccSts::INIT);
    if (desired_state == AccSts::INIT) {
        return bm;
    }

    if (bm.Update(
              {
                  .cmd = AccCmd::STARTUP,
                  .pack_soc = 20.0,
              },
              time_ms)
            .status == AccSts::LOW_SOC) {
        return bm;
    }

    // Transition to CLOSE_NEG
    time_ms += 10;
    auto output2 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_OPEN,
                    .negative = IS_OPEN,
                    .positive = IS_OPEN,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output2.status == AccSts::STARTUP_CLOSE_NEG);
    if (desired_state == AccSts::STARTUP_CLOSE_NEG) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_NEG
    time_ms += 10;
    auto output3 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback{
                .precharge = IS_OPEN,
                .negative = IS_CLOSED,
                .positive = IS_OPEN,
            },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output3.status == AccSts::STARTUP_HOLD_CLOSE_NEG);
    if (desired_state == AccSts::STARTUP_HOLD_CLOSE_NEG) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_PRECHARGE
    time_ms += 100;
    auto output4 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback{
                .precharge = IS_CLOSED,
                .negative = IS_CLOSED,
                .positive = IS_OPEN,
            },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output4.status == AccSts::STARTUP_CLOSE_PRECHARGE);
    if (desired_state == AccSts::STARTUP_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_PRECHARGE
    time_ms += 10;
    auto output5 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_CLOSED,
                    .negative = IS_CLOSED,
                    .positive = IS_OPEN,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output5.status == AccSts::STARTUP_HOLD_CLOSE_PRECHARGE);
    if (desired_state == AccSts::STARTUP_HOLD_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_POS
    time_ms += 10000;
    auto output6 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_CLOSED,
                    .negative = IS_CLOSED,
                    .positive = IS_CLOSED,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output6.status == AccSts::STARTUP_CLOSE_POS);
    if (desired_state == AccSts::STARTUP_CLOSE_POS) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_POS
    time_ms += 10;
    auto output7 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_CLOSED,
                    .negative = IS_CLOSED,
                    .positive = IS_CLOSED,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output7.status == AccSts::STARTUP_HOLD_CLOSE_POS);
    if (desired_state == AccSts::STARTUP_HOLD_CLOSE_POS) {
        return bm;
    }

    // Transition to STARTUP_OPEN_PRECHARGE
    time_ms += 100;
    auto output8 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_OPEN,
                    .negative = IS_CLOSED,
                    .positive = IS_CLOSED,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output8.status == AccSts::STARTUP_OPEN_PRECHARGE);
    if (desired_state == AccSts::STARTUP_OPEN_PRECHARGE) {
        return bm;
    }

    // Transition to RUNNING
    time_ms += 10;
    auto output9 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback =
                {
                    .precharge = IS_OPEN,
                    .negative = IS_CLOSED,
                    .positive = IS_CLOSED,
                },
            .pack_soc = 50.0,
        },
        time_ms);

    assert(output9.status == AccSts::RUNNING);
    if (desired_state == AccSts::RUNNING) {
        return bm;
    }

    // Transition to LOW_SOC
    auto output10 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .pack_soc = 20.0,
        },
        time_ms);

    assert(output10.status == AccSts::LOW_SOC);
    return bm;
}

std::string BmStatusToString(AccSts status) {
    switch (status) {
        case AccSts::INIT:
            return "INIT";
        case AccSts::STARTUP_CLOSE_NEG:
            return "STARTUP_CLOSE_NEG";
        case AccSts::STARTUP_CLOSE_PRECHARGE:
            return "STARTUP_CLOSE_PRECHARGE";
        case AccSts::STARTUP_CLOSE_POS:
            return "STARTUP_CLOSE_POS";
        case AccSts::STARTUP_OPEN_PRECHARGE:
            return "STARTUP_OPEN_PRECHARGE";
        case AccSts::RUNNING:
            return "RUNNING";
        case AccSts::LOW_SOC:
            return "LOW_SOC";
        default:
            return "UNKNOWN";
    }
}

TEST(BatteryMonitor, LowSocTransitions) {
    std::vector<AccSts> states = {
        AccSts::STARTUP_CLOSE_NEG, AccSts::STARTUP_CLOSE_PRECHARGE,
        AccSts::STARTUP_CLOSE_POS, AccSts::STARTUP_OPEN_PRECHARGE};
    int time_ms;

    for (AccSts state : states) {
        Accumulator bm = CycleToState(state, time_ms);

        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = AccCmd::STARTUP,
                .pack_soc = 20.0,
            },
            time_ms);

        ASSERT_EQ(output.status, AccSts::LOW_SOC)
            << "Failed transition from" << BmStatusToString(state)
            << " to LOW_SOC.";
    }
}

TEST(BatteryMonitor, StateTransitions) {
    using enum ContactorFeedback::State;
    using enum AccSts;

    Accumulator bm{};
    int time_ms = 0;

    auto output1 = bm.Update(
        {
            .cmd = AccCmd::INIT,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_OPEN,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output1.status, INIT) << "Should start in Initialized state";

    time_ms += 10;
    auto output2 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_OPEN,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output2.status, STARTUP_ENSURE_OPEN);

    time_ms += 10;
    auto output2b = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_CLOSED,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output2b.status, STARTUP_ENSURE_OPEN)
        << "Shouldn't transition until all are OPEN.";

    time_ms += 10;
    auto output2c = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_OPEN,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output2c.status, STARTUP_CLOSE_NEG)
        << "Should proceed once contactora are OPEN.";

    time_ms += 10;
    auto output3 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_CLOSED,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output3.status, STARTUP_HOLD_CLOSE_NEG)
        << "Failed CLOSE_NEG to HOLD_CLOSE_NEG";

    time_ms += 100;
    auto output4 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_CLOSED,
                         .negative = IS_CLOSED,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output4.status, STARTUP_CLOSE_PRECHARGE)
        << "Failed HOLD_CLOSE_NEG to CLOSE_PRECHARGE";

    time_ms += 10;
    auto output5 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_CLOSED,
                         .negative = IS_CLOSED,
                         .positive = IS_OPEN},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output5.status, STARTUP_HOLD_CLOSE_PRECHARGE)
        << "Failed CLOSE_PRECHARGE to HOLD_CLOSE_PRECHARGE";

    time_ms += 10000;
    auto output6 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_CLOSED,
                         .negative = IS_CLOSED,
                         .positive = IS_CLOSED},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output6.status, STARTUP_CLOSE_POS)
        << "Failed HOLD_CLOSE_PRECHARGE to CLOSE_POS";

    time_ms += 10;
    auto output7 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_CLOSED,
                         .negative = IS_CLOSED,
                         .positive = IS_CLOSED},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output7.status, STARTUP_HOLD_CLOSE_POS)
        << "Failed CLOSE_POS to HOLD_CLOSE_POS";

    time_ms += 100;
    auto output8 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_CLOSED,
                         .positive = IS_CLOSED},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output8.status, STARTUP_OPEN_PRECHARGE)
        << "Failed HOLD_CLOSE_POS to OPEN_PRECHARGE";

    time_ms += 10;
    auto output9 = bm.Update(
        {
            .cmd = AccCmd::STARTUP,
            .feedback = {.precharge = IS_OPEN,
                         .negative = IS_CLOSED,
                         .positive = IS_CLOSED},
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output9.status, RUNNING) << "Failed OPEN_PRECHARGE to RUNNING";
}

TEST(BatteryMonitor, BooleanValues) {
    // Make sure nobody changes these!
    EXPECT_EQ(static_cast<bool>(ContactorFeedback::State::IS_CLOSED), false);
    EXPECT_EQ(static_cast<bool>(ContactorFeedback::State::IS_OPEN), true);

    EXPECT_EQ(static_cast<bool>(ContactorCommand::State::CLOSE), true);
    EXPECT_EQ(static_cast<bool>(ContactorCommand::State::OPEN), false);
}