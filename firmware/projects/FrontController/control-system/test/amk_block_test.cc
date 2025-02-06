#include "amk_block.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>

#include "control-system/enums.hpp"

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

template <SetPoints SP>
void assert_setpoint_equal(SP actual_sp, SP expected_sp) {
    assert(actual_sp.amk_b_inverter_on == expected_sp.amk_b_inverter_on);
    assert(actual_sp.amk_b_dc_on == expected_sp.amk_b_dc_on);
    assert(actual_sp.amk_b_enable == expected_sp.amk_b_enable);
    assert(actual_sp.amk_b_error_reset == expected_sp.amk_b_error_reset);
    assert(actual_sp.amk__target_velocity == expected_sp.amk__target_velocity);
    assert(actual_sp.amk__torque_limit_positiv ==
           expected_sp.amk__torque_limit_positiv);
    assert(actual_sp.amk__torque_limit_negativ ==
           expected_sp.amk__torque_limit_negativ);
}

// Tests the whole control model sequence from start to end
void test_normal_sequence() {
    UpdateMotorOutput<DummySetpoints1> output;
    AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
        AmkStates::MOTOR_OFF_WAITING_FOR_GOV);

    // Inputs to change and use in each Update call
    DummyActualValues1 actual_values{};
    MotorInput motor_input = MotorInput{.speed_request = 1.5,
                                        .torque_limit_positive = 2.5,
                                        .torque_limit_negative = 3.5};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    DummySetpoints1 expected_setpoints;

    // Test transition MOTOR_OFF_WAITING_FOR_GOV to STARTUP_SYS_READY
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test transition STARTUP_SYS_READY to STARTUP_TOGGLE_D_CON
    actual_values.amk_b_system_ready = true;
    expected_setpoints.amk_b_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test transition STARTUP_TOGGLE_D_CON to STARTUP_ENFORCE_SETPOINTS_ZERO
    actual_values.amk_b_dc_on = true;
    actual_values.amk_b_quit_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test NO transition in STARTUP_TOGGLE_D_CON with not enough time elapsed
    time_ms += 50;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to STARTUP_COMMAND_ON
    time_ms += 50;
    expected_setpoints.amk_b_enable = true;
    expected_setpoints.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test transition STARTUP_COMMAND_ON to READY
    actual_values.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::READY);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == true);
    }

    // Test transition READY to RUNNING
    actual_values.amk_b_quit_inverter_on = true;
    expected_setpoints.amk__target_velocity = 1.5;
    expected_setpoints.amk__torque_limit_positiv = 2.5;
    expected_setpoints.amk__torque_limit_negativ = 3.5;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == true);
    }

    // Test transition RUNNING to SHUTDOWN
    cmd = MiCmd::SHUTDOWN;
    expected_setpoints.amk__target_velocity = 0;
    expected_setpoints.amk__torque_limit_positiv = 0;
    expected_setpoints.amk__torque_limit_negativ = 0;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test NO transition in SHUTDOWN with not enough time elapsed
    time_ms += 250;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    // Test transition SHUTDOWN to MOTOR_OFF_WAITING_FOR_GOV
    time_ms += 250;
    expected_setpoints = DummySetpoints1{};
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::OFF);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }
}

void test_error_detected_state() {
    // Define inputs/output
    UpdateMotorOutput<DummySetpoints1> output;
    DummyActualValues1 actual_values;
    MotorInput motor_input = MotorInput{};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Set inputs to cause an error for testing
    actual_values.amk_b_error = true;

    // Test transition STARTUP_SYS_READY to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::STARTUP_SYS_READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    // Test transition STARTUP_TOGGLE_D_CON to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::STARTUP_TOGGLE_D_CON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    // Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::STARTUP_ENFORCE_SETPOINTS_ZERO);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    // Test transition STARTUP_COMMAND_ON to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::STARTUP_COMMAND_ON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    // Test transition READY to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    // Test transition RUNNING to ERROR_DETECTED
    {
        AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
            AmkStates::RUNNING);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }
}

void test_error_sequence() {
    UpdateMotorOutput<DummySetpoints1> output;
    AmkManager<DummyActualValues1, DummySetpoints1> amk_manager(
        AmkStates::ERROR_DETECTED);

    // Inputs
    DummyActualValues1 actual_values;
    MotorInput motor_input = MotorInput{};
    MiCmd cmd = MiCmd::ERR_RESET;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    DummySetpoints1 expected_setpoints;

    // Test transition ERROR_DETECTED to ERROR_RESET_ENFORCE_SETPOINTS_ZERO
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test transition ERROR_RESET_ENFORCE_SETPOINTS_ZERO to
    // ERROR_RESET_TOGGLE_ENABLE
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test NO transition in ERROR_RESET_TOGGLE_ENABLE with not enough time
    time_ms += 250;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test transition ERROR_RESET_TOGGLE_ENABLE to ERROR_RESET_SEND_RESET
    time_ms += 250;
    expected_setpoints.amk_b_error_reset = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test NO transition in ERROR_RESET_SEND_RESET
    time_ms += 250;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test transition ERROR_RESET_SEND_RESET to ERROR_RESET_TOGGLE_RESET
    time_ms += 250;
    expected_setpoints.amk_b_error_reset = false;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }

    // Test transition ERROR_RESET_TOGGLE_RESET to MOTOR_OFF_WAITING_FOR_GOV
    actual_values.amk_b_system_ready = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::OFF);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
    }
}

void AmkTest() {
    test_normal_sequence();
    test_error_detected_state();
    test_error_sequence();

    std::cout << "All AMK tests passed!" << std::endl;
}