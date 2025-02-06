#include "amk_block.hpp"

#include <cassert>
#include <iostream>

#include "control-system/enums.hpp"

using namespace generated::can;

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
    UpdateMotorOutput<AMK0_SetPoints1> output;
    AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
        AmkStates::MOTOR_OFF_WAITING_FOR_GOV);

    // Inputs to change and use in each Update call
    AMK0_ActualValues1 actual_values;
    MotorInput motor_input = MotorInput{.speed_request = 1.5,
                                        .torque_limit_positive = 2.5,
                                        .torque_limit_negative = 3.5};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    AMK0_SetPoints1 expected_setpoints;

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
    expected_setpoints = AMK0_SetPoints1{};
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
    UpdateMotorOutput<AMK0_SetPoints1> output;
    AMK0_ActualValues1 actual_values;
    MotorInput motor_input = MotorInput{};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Set inputs to cause an error for testing
    actual_values.amk_b_error = true;

    // Test transition STARTUP_SYS_READY to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::STARTUP_SYS_READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }

    // Test transition STARTUP_TOGGLE_D_CON to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::STARTUP_TOGGLE_D_CON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }

    // Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::STARTUP_ENFORCE_SETPOINTS_ZERO);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }

    // Test transition STARTUP_COMMAND_ON to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::STARTUP_COMMAND_ON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }

    // Test transition READY to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }

    // Test transition RUNNING to ERROR_DETECTED
    {
        AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
            AmkStates::RUNNING);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERROR);
    }
}

void test_error_sequence() {
    UpdateMotorOutput<AMK0_SetPoints1> output;
    AmkManager<AMK0_ActualValues1, AMK0_SetPoints1> amk_manager(
        AmkStates::ERROR_DETECTED);

    // Inputs
    AMK0_ActualValues1 actual_values;
    MotorInput motor_input = MotorInput{};
    MiCmd cmd = MiCmd::ERR_RESET;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    AMK0_SetPoints1 expected_setpoints;

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