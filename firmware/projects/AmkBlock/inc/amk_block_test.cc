#include "amk_block.hpp"

#include <cassert>
#include <iostream>

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
    AmkBlock amk(AmkStates::MOTOR_OFF_WAITING_FOR_GOV);
    int time_ms = 0;

    // Input/output to change and use in each Update call
    MotorInput motor_input = MotorInput{.speed_request = 1.5,
                                        .torque_limit_positive = 2.5,
                                        .torque_limit_negative = 3.5};
    AmkInput input = {.cmd = MiCmd::STARTUP,
                      .left_actual1 = generated::can::AMK0_ActualValues1{},
                      .left_actual2 = generated::can::AMK0_ActualValues2{},
                      .right_actual1 = generated::can::AMK1_ActualValues1{},
                      .right_actual2 = generated::can::AMK1_ActualValues2{},
                      .left_motor_input = motor_input,
                      .right_motor_input = motor_input};
    AmkOutput output;

    // Expected setpoints to change and use in assert calls
    generated::can::AMK0_SetPoints1 expected_left_setpoints{};
    generated::can::AMK1_SetPoints1 expected_right_setpoints{};

    // Test transition MOTOR_OFF_WAITING_FOR_GOV to STARTUP_SYS_READY
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::STARTUP);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    // Test transition STARTUP_SYS_READY to STARTUP_TOGGLE_D_CON
    input.left_actual1.amk_b_system_ready = 1;
    input.right_actual1.amk_b_system_ready = 1;
    expected_left_setpoints.amk_b_dc_on = 1;
    expected_right_setpoints.amk_b_dc_on = 1;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::STARTUP);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    // Test transition STARTUP_TOGGLE_D_CON to STARTUP_ENFORCE_SETPOINTS_ZERO
    input.left_actual1.amk_b_dc_on = 1;
    input.right_actual1.amk_b_dc_on = 1;
    input.left_actual1.amk_b_quit_dc_on = 1;
    input.right_actual1.amk_b_quit_dc_on = 1;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::STARTUP);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    time_ms += 100;

    // Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to STARTUP_COMMAND_ON
    expected_left_setpoints.amk_b_enable = 1;
    expected_right_setpoints.amk_b_enable = 1;
    expected_left_setpoints.amk_b_inverter_on = 1;
    expected_right_setpoints.amk_b_inverter_on = 1;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::STARTUP);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    // Test transition STARTUP_COMMAND_ON to READY
    input.left_actual1.amk_b_inverter_on = 1;
    input.right_actual1.amk_b_inverter_on = 1;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::READY);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 1);
    }

    // Test transition READY to RUNNING
    input.left_actual1.amk_b_quit_inverter_on = 1;
    input.right_actual1.amk_b_quit_inverter_on = 1;
    expected_left_setpoints.amk__target_velocity = 1.5;
    expected_left_setpoints.amk__torque_limit_positiv = 2.5;
    expected_left_setpoints.amk__torque_limit_negativ = 3.5;
    expected_right_setpoints.amk__target_velocity = 1.5;
    expected_right_setpoints.amk__torque_limit_positiv = 2.5;
    expected_right_setpoints.amk__torque_limit_negativ = 3.5;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::RUNNING);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 1);
    }

    // Test transition RUNNING to SHUTDOWN
    input.cmd = MiCmd::SHUTDOWN;
    expected_left_setpoints.amk__target_velocity = 0;
    expected_left_setpoints.amk__torque_limit_positiv = 0;
    expected_left_setpoints.amk__torque_limit_negativ = 0;
    expected_right_setpoints.amk__target_velocity = 0;
    expected_right_setpoints.amk__torque_limit_positiv = 0;
    expected_right_setpoints.amk__torque_limit_negativ = 0;
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::RUNNING);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    time_ms += 500;

    // Test transition SHUTDOWN to MOTOR_OFF_WAITING_FOR_GOV
    expected_left_setpoints = generated::can::AMK0_SetPoints1{};
    expected_right_setpoints = generated::can::AMK1_SetPoints1{};
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::OFF);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }
}

int run_tests() {
    test_normal_sequence();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}