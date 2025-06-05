#include <cassert>
#include <cstdint>

#include "../enums.hpp"
#include "gtest/gtest.h"
#include "motor_interface.hpp"

// Define Dummy CAN messages to make the test module independent
// clang-format off
struct DummyActualValues1 {
    bool AMK_bSystemReady() const { return amk_b_system_ready; }
    bool AMK_bError() const { return amk_b_error; }
    bool AMK_bWarn() const { return amk_b_warn; }
    bool AMK_bQuitDcOn() const { return amk_b_quit_dc_on; }
    bool AMK_bDcOn() const { return amk_b_dc_on; }
    bool AMK_bQuitInverterOn() const { return amk_b_quit_inverter_on; }
    bool AMK_bInverterOn() const { return amk_b_inverter_on; }
    bool AMK_bDerating() const { return amk_b_derating; }
    int16_t AMK_ActualVelocity() const { return amk_actual_velocity; }
    int16_t AMK_TorqueCurrent() const { return amk_torque_current; }
    int16_t AMK_MagnetizingCurrent() const { return amk_magnetizing_current; }

    bool amk_b_system_ready; 
    bool amk_b_error; 
    bool amk_b_warn; 
    bool amk_b_quit_dc_on; 
    bool amk_b_dc_on; 
    bool amk_b_quit_inverter_on; 
    bool amk_b_inverter_on; 
    bool amk_b_derating; 
    int16_t amk_actual_velocity; 
    int16_t amk_torque_current; 
    int16_t amk_magnetizing_current; 
};

struct DummySetpoints1 {
    bool amk_b_inverter_on;
    bool amk_b_dc_on;
    bool amk_b_enable;
    bool amk_b_error_reset;
    int16_t amk__target_velocity;
    int16_t amk__torque_limit_positiv;
    int16_t amk__torque_limit_negativ;
};
// clang-format on

using AmkMgr = AmkManager<DummyActualValues1, DummySetpoints1>;

// Might be better to define operator== for SP than use this macro
#define assert_setpoint_equal(actual_sp, expected_sp)                      \
    EXPECT_EQ(actual_sp.amk_b_inverter_on, expected_sp.amk_b_inverter_on); \
    EXPECT_EQ(actual_sp.amk_b_dc_on, expected_sp.amk_b_dc_on);             \
    EXPECT_EQ(actual_sp.amk_b_enable, expected_sp.amk_b_enable);           \
    EXPECT_EQ(actual_sp.amk_b_error_reset, expected_sp.amk_b_error_reset); \
    EXPECT_EQ(actual_sp.amk__target_velocity,                              \
              expected_sp.amk__target_velocity);                           \
    EXPECT_EQ(actual_sp.amk__torque_limit_positiv,                         \
              expected_sp.amk__torque_limit_positiv);                      \
    EXPECT_EQ(actual_sp.amk__torque_limit_negativ,                         \
              expected_sp.amk__torque_limit_negativ);

AmkManager<DummyActualValues1, DummySetpoints1> CycleToState(
    AmkManagerBase::FsmState desired_state) {
    using enum AmkManagerBase::FsmState;

    auto possible_states = {
        MOTOR_OFF_WAITING_FOR_GOV,
        STARTUP_SYS_READY,
        STARTUP_TOGGLE_D_CON,
        STARTUP_ENFORCE_SETPOINTS_ZERO,
        STARTUP_COMMAND_ON,
        READY,
        RUNNING,
        SHUTDOWN,
    };

    bool desired_state_is_supported = false;
    for (auto state : possible_states) {
        if (desired_state == state) {
            desired_state_is_supported = true;
            break;
        }
    }
    assert(desired_state_is_supported);

    AmkMgr mgr{};
    if (desired_state == MOTOR_OFF_WAITING_FOR_GOV) return mgr;

    DummyActualValues1 val1{};
    AmkMgr::Output out;
    AmkMgr::Request motor{};
    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 0);
    assert(out.fsm_state == STARTUP_SYS_READY);
    if (desired_state == STARTUP_SYS_READY) return mgr;

    val1.amk_b_system_ready = true;
    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 1);
    assert(out.fsm_state == STARTUP_TOGGLE_D_CON);
    if (desired_state == STARTUP_TOGGLE_D_CON) return mgr;

    val1.amk_b_dc_on = true;
    val1.amk_b_quit_dc_on = true;
    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 2);
    assert(out.fsm_state == STARTUP_ENFORCE_SETPOINTS_ZERO);
    if (desired_state == STARTUP_ENFORCE_SETPOINTS_ZERO) return mgr;

    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 150);
    assert(out.fsm_state == STARTUP_COMMAND_ON);
    if (desired_state == STARTUP_COMMAND_ON) return mgr;

    val1.amk_b_inverter_on = true;
    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 151);
    assert(out.fsm_state == READY);
    if (desired_state == READY) return mgr;

    val1.amk_b_quit_inverter_on = true;
    val1.amk_b_error = false;
    out = mgr.UpdateMotor(val1, motor, motor::Command::STARTUP, 152);
    assert(out.fsm_state == RUNNING);
    if (desired_state == RUNNING) return mgr;

    out = mgr.UpdateMotor(val1, motor, motor::Command::SHUTDOWN, 152);
    assert(out.fsm_state == SHUTDOWN);
    return mgr;
}

// Tests the whole control model sequence from start to end
TEST(AmkMotor, NormalSequence) {
    AmkMgr::Output output;
    AmkMgr amk_manager{};

    // Inputs to change and use in each Update call
    DummyActualValues1 actual_values{};
    AmkMgr::Request motor_input{.speed_request = 15,
                                .torque_limit_positive = 25,
                                .torque_limit_negative = 35};
    motor::Command cmd = motor::Command::STARTUP;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    DummySetpoints1 expected_setpoints{};

    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    actual_values.amk_b_system_ready = true;
    expected_setpoints.amk_b_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    actual_values.amk_b_dc_on = true;
    actual_values.amk_b_quit_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    time_ms += 50;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    time_ms += 50;
    expected_setpoints.amk_b_enable = true;
    expected_setpoints.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    actual_values.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::READY);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_TRUE(output.inverter_enable);
    }

    actual_values.amk_b_quit_inverter_on = true;
    expected_setpoints.amk__target_velocity = 15;
    expected_setpoints.amk__torque_limit_positiv = 25;
    expected_setpoints.amk__torque_limit_negativ = 35;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_TRUE(output.inverter_enable);
    }

    cmd = motor::Command::SHUTDOWN;
    expected_setpoints.amk__target_velocity = 0;
    expected_setpoints.amk__torque_limit_positiv = 0;
    expected_setpoints.amk__torque_limit_negativ = 0;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    time_ms += 250;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }

    time_ms += 250;
    expected_setpoints = DummySetpoints1{};
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::OFF);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        ASSERT_FALSE(output.inverter_enable);
    }
}

TEST(AmkMotor, ErrorDetectedState) {
    using enum AmkManagerBase::FsmState;

    // Define inputs/output
    AmkMgr::Output output;
    DummyActualValues1 actual_values;
    AmkMgr::Request motor_input{};
    motor::Command cmd = motor::Command::STARTUP;
    int time_ms = 0;

    // Set inputs to cause an error for testing
    actual_values.amk_b_error = true;

    {
        auto amk_manager = CycleToState(STARTUP_SYS_READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }

    {
        auto amk_manager = CycleToState(STARTUP_TOGGLE_D_CON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }

    {
        auto amk_manager = CycleToState(STARTUP_ENFORCE_SETPOINTS_ZERO);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }

    {
        auto amk_manager = CycleToState(STARTUP_COMMAND_ON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }

    {
        auto amk_manager = CycleToState(READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }

    {
        auto amk_manager = CycleToState(RUNNING);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        ASSERT_EQ(output.status, MiSts::ERR);
    }
}

TEST(AmkMotor, ErrorSequence) {
    using enum AmkManagerBase::FsmState;
    auto amk_manager = CycleToState(RUNNING);

    AmkMgr::Output output;

    // Inputs
    DummyActualValues1 actual_values;
    AmkMgr::Request motor_input{};
    int time_ms = 0;

    // put amk into error state
    output = amk_manager.UpdateMotor({.amk_b_error = true}, motor_input,
                                     motor::Command::ERR_RESET, time_ms);
    ASSERT_EQ(output.fsm_state, ERROR_DETECTED);
    ASSERT_EQ(output.status, MiSts::ERR);

    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        EXPECT_EQ(output.setpoints.amk__target_velocity, 0);
        EXPECT_EQ(output.setpoints.amk__torque_limit_positiv, 0);
        EXPECT_EQ(output.setpoints.amk__torque_limit_positiv, 0);
        EXPECT_FALSE(output.setpoints.amk_b_inverter_on);
    }

    {
        output = amk_manager.UpdateMotor({.amk_b_quit_inverter_on = false},
                                         motor_input, motor::Command::ERR_RESET,
                                         time_ms);
        ASSERT_EQ(output.fsm_state, ERROR_RESET_TOGGLE_ENABLE);
        EXPECT_FALSE(output.setpoints.amk_b_enable);
    }

    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        ASSERT_EQ(output.fsm_state, ERROR_RESET_TOGGLE_ENABLE);
    }

    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        ASSERT_EQ(output.fsm_state, ERROR_RESET_SEND_RESET);
        EXPECT_TRUE(output.setpoints.amk_b_error_reset);
    }

    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        ASSERT_EQ(output.fsm_state, ERROR_RESET_SEND_RESET);
    }

    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        ASSERT_EQ(output.fsm_state, ERROR_RESET_TOGGLE_RESET);
        EXPECT_FALSE(output.setpoints.amk_b_error_reset);
    }

    actual_values.amk_b_system_ready = true;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         motor::Command::ERR_RESET, time_ms);
        ASSERT_EQ(output.status, MiSts::OFF);
        EXPECT_FALSE(output.setpoints.amk_b_inverter_on);
        EXPECT_FALSE(output.setpoints.amk_b_dc_on);
        EXPECT_FALSE(output.setpoints.amk_b_enable);
        EXPECT_FALSE(output.setpoints.amk_b_error_reset);
        EXPECT_FALSE(output.setpoints.amk__target_velocity);
        EXPECT_FALSE(output.setpoints.amk__torque_limit_positiv);
        EXPECT_FALSE(output.setpoints.amk__torque_limit_positiv);
        ASSERT_FALSE(output.inverter_enable);
    }
}
