#include "control-system/battery_monitor.h"
#include "control-system/enums.hpp"
#include "gtest/gtest.h"

// this is not a test itself, rather it produces a BatteryMonitor object in a
// specific state to make it easier to run other tests
BatteryMonitor CycleToState(BmSts desired_state, int& time_ms) {
    using enum ContactorState;

    BatteryMonitor bm{};
    time_ms = 0;

    // Initialize the BatteryMonitor
    bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::INIT) {
        return bm;
    }

    if (bm.Update(
              {
                  .cmd = BmCmd::STARTUP,
                  .pack_soc = 20.0,
              },
              time_ms)
            .status == BmSts::LOW_SOC) {
        return bm;
    }

    // Transition to CLOSE_NEG
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_NEG) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_NEG
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_NEG) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_PRECHARGE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_PRECHARGE
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_PRECHARGE) {
        return bm;
    }

    // Transition to STARTUP_CLOSE_POS
    time_ms += 6500;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_CLOSE_POS) {
        return bm;
    }

    // Transition to HOLD_STARTUP_CLOSE_POS
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_HOLD_CLOSE_POS) {
        return bm;
    }

    // Transition to STARTUP_OPEN_PRECHARGE
    time_ms += 100;
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::STARTUP_OPEN_PRECHARGE) {
        return bm;
    }

    // Transition to RUNNING
    time_ms += 10;
    bm.Update(
        {
            .cmd = BmCmd::RUNNING,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);

    if (desired_state == BmSts::RUNNING) {
        return bm;
    }

    // Transition to LOW_SOC
    bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .pack_soc = 20.0,
        },
        time_ms);

    return bm;
}

std::string BmStatusToString(BmSts status) {
    switch (status) {
        case BmSts::INIT:
            return "INIT";
        case BmSts::STARTUP_CLOSE_NEG:
            return "STARTUP_CLOSE_NEG";
        case BmSts::STARTUP_CLOSE_PRECHARGE:
            return "STARTUP_CLOSE_PRECHARGE";
        case BmSts::STARTUP_CLOSE_POS:
            return "STARTUP_CLOSE_POS";
        case BmSts::STARTUP_OPEN_PRECHARGE:
            return "STARTUP_OPEN_PRECHARGE";
        case BmSts::RUNNING:
            return "RUNNING";
        case BmSts::LOW_SOC:
            return "LOW_SOC";
        default:
            return "UNKNOWN";
    }
}

TEST(BatteryMonitor, LowSocTransitions) {
    using enum ContactorState;
    std::vector<BmSts> states = {
        BmSts::STARTUP_CLOSE_NEG, BmSts::STARTUP_CLOSE_PRECHARGE,
        BmSts::STARTUP_CLOSE_POS, BmSts::STARTUP_OPEN_PRECHARGE};
    int time_ms;

    for (BmSts state : states) {
        BatteryMonitor bm = CycleToState(state, time_ms);

        time_ms += 10;
        auto output = bm.Update(
            {
                .cmd = BmCmd::STARTUP,
                .pack_soc = 20.0,
            },
            time_ms);

        ASSERT_EQ(output.status, BmSts::LOW_SOC)
            << "Failed transition from" << BmStatusToString(state)
            << " to LOW_SOC.";
    }
}

TEST(BatteryMonitor, StateTransitions) {
    using enum ContactorState;
    using enum BmSts;

    BatteryMonitor bm{};
    int time_ms = 0;

    auto output1 = bm.Update(
        {
            .cmd = BmCmd::INIT,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output1.status, INIT) << "Should start in Initialized state";

    time_ms += 10;
    auto output2 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = OPEN,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output2.status, STARTUP_CLOSE_NEG) << "Failed INIT to CLOSE_NEG";

    time_ms += 10;
    auto output3 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output3.status, STARTUP_HOLD_CLOSE_NEG)
        << "Failed CLOSE_NEG to HOLD_CLOSE_NEG";

    time_ms += 100;
    auto output4 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output4.status, STARTUP_CLOSE_PRECHARGE)
        << "Failed HOLD_CLOSE_NEG to CLOSE_PRECHARGE";

    time_ms += 10;
    auto output5 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = OPEN,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output5.status, STARTUP_HOLD_CLOSE_PRECHARGE)
        << "Failed CLOSE_PRECHARGE to HOLD_CLOSE_PRECHARGE";

    time_ms += 6500;
    auto output6 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output6.status, STARTUP_CLOSE_POS)
        << "Failed HOLD_CLOSE_PRECHARGE to CLOSE_POS";

    time_ms += 10;
    auto output7 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = CLOSED,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output7.status, STARTUP_HOLD_CLOSE_POS)
        << "Failed CLOSE_POS to HOLD_CLOSE_POS";

    time_ms += 100;
    auto output8 = bm.Update(
        {
            .cmd = BmCmd::STARTUP,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output8.status, STARTUP_OPEN_PRECHARGE)
        << "Failed HOLD_CLOSE_POS to OPEN_PRECHARGE";

    time_ms += 10;
    auto output9 = bm.Update(
        {
            .cmd = BmCmd::RUNNING,
            .precharge_contactor_states = OPEN,
            .pos_contactor_states = CLOSED,
            .neg_contactor_states = CLOSED,
            .pack_soc = 50.0,
        },
        time_ms);
    ASSERT_EQ(output9.status, RUNNING) << "Failed OPEN_PRECHARGE to RUNNING";
}
