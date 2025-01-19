#include <cassert>
#include <iostream>

#include "amk_block.hpp"

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
    AmkInput input = {.cmd = MiCmd::STARTUP,
                      .left_actual1 = generated::can::AMK0_ActualValues1{},
                      .left_actual2 = generated::can::AMK0_ActualValues2{},
                      .right_actual1 = generated::can::AMK1_ActualValues1{},
                      .right_actual2 = generated::can::AMK1_ActualValues2{},
                      .left_motor_input = MotorInput{},
                      .right_motor_input = MotorInput{}};
    AmkOutput output;

    // Expected setpoints to change and use in assert calls
    generated::can::AMK0_SetPoints1 expected_left_setpoints{};
    generated::can::AMK1_SetPoints1 expected_right_setpoints{};

    // Test transition from MOTOR_OFF_WAITING_FOR_GOV to STARTUP_SYS_READY
    {
        output = amk.Update(input, time_ms);
        assert(output.status == MiStatus::STARTUP);
        assert_setpoint_equal(output.left_setpoints, expected_left_setpoints);
        assert_setpoint_equal(output.right_setpoints, expected_right_setpoints);
        assert(output.inverter_enable == 0);
    }

    time_ms += 10;

    // Test transition from STARTUP_SYS_READY to STARTUP_TOGGLE_D_CON
    // {
    //     output = amk.Update(input, time_ms);
    //     std::cout << static_cast<int>(output.status) << std::endl;
    //     assert(output.status == MiStatus::STARTUP);
    //     assert_setpoint_equal(output.left_setpoints,
    //     expected_left_setpoints);
    //     assert_setpoint_equal(output.right_setpoints,
    //     expected_right_setpoints); assert(output.inverter_enable == 0);
    // }

    // assert(output2.status == MiStatus::STARTUP);
}

int run_tests() {
    test_normal_sequence();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}