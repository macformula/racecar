/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "simp_vd_interface.hpp"
#include "shared/controls/testing.h"

#include <cassert>
#include <iostream>

void test_1(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 1 loading" << std::endl;

    int time_ms = 0;

    VdOutput output_1 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 0.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 0.0f,
            .wheel_speed_rr = 0.0f,
            .wheel_speed_lf = 0.0f,
            .wheel_speed_rf = 0.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_1_expected{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 1: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_1.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_1.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_1.lm_torque_limit_positive, output_1_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_1.lm_torque_limit_positive, output_1_expected.lm_torque_limit_positive);
}

void test_2(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 2 loading" << std::endl;

    int time_ms = 0;

    VdOutput output_2 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 100.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 0.0f,
            .wheel_speed_rr = 0.0f,
            .wheel_speed_lf = 0.0f,
            .wheel_speed_rf = 0.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_2_expected{
        // Expected to calculate torque limit of 100.0f for both motors given test 2 input
        // Expected output of 50.0f after accounting for running avg of motor torque and test 1
        .lm_torque_limit_positive = 50.0f,
        .rm_torque_limit_positive = 50.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 2: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_2.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_2.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_2.lm_torque_limit_positive, output_2_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_2.lm_torque_limit_positive, output_2_expected.lm_torque_limit_positive);
}

void test_3(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 3 loading" << std::endl;

    int time_ms = 0;

    VdOutput output_3 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 100.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 25.0f,
            .wheel_speed_lr = 0.0f,
            .wheel_speed_rr = 0.0f,
            .wheel_speed_lf = 0.0f,
            .wheel_speed_rf = 0.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_3_expected{
        // Expected to calculate lm=100.0f, rm=68.3f torque limits given test 3 input
        // Expected output of lm=66.7, rm=66.7*.683=45.5f after accounting for running avg of motor torque and prev tests
        .lm_torque_limit_positive = 66.66667f,
        .rm_torque_limit_positive = 45.53336f, // 5 decimal places due to tolerance
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 3: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_3.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_3.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_3.lm_torque_limit_positive, output_3_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_3.lm_torque_limit_positive, output_3_expected.lm_torque_limit_positive);
}

void test_4(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 4 loading" << std::endl;

    int time_ms = 0;

    VdOutput output_4 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 50.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 15.0f, // actual slip 0.5
            .wheel_speed_rr = 15.0f,
            .wheel_speed_lf = 10.0f,
            .wheel_speed_rf = 10.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_4_expected{
        // Expected to calculate lm=rm=50.0f torque limit given test 4 input
        // Moving avg: (prev torque limits) / count = (0 + 100 + 100 + 50) / 4 = 62.5f
        .lm_torque_limit_positive = 62.5f,
        .rm_torque_limit_positive = 62.5f, // tc scale factor = 1, slipping for < 50ms
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 4: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_4.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_4.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_4.lm_torque_limit_positive, output_4_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_4.rm_torque_limit_positive, output_4_expected.rm_torque_limit_positive);
}

void test_5(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 5 loading" << std::endl;

    int time_ms = 55;

    VdOutput output_5 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 50.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 15.0f, // actual slip 0.5
            .wheel_speed_rr = 15.0f,
            .wheel_speed_lf = 10.0f,
            .wheel_speed_rf = 10.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_5_expected{
        // Expected to calculate lm=rm=0.0f torque limit given test 5 input
        // Moving avg: (prev torque limits) / count = (0 + 100 + 100 + 50 + 0) / 5 = 50.0f
        .lm_torque_limit_positive = 50.0f,
        .rm_torque_limit_positive = 50.0f, // tc scale factor = 0, slipping for > 50ms
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 5: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_5.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_5.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_5.lm_torque_limit_positive, output_5_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_5.rm_torque_limit_positive, output_5_expected.rm_torque_limit_positive);
}

void test_6(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 6 loading" << std::endl;

    int time_ms = 0;

    VdOutput output_6 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 0.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 0.0f,
            .wheel_speed_rr = 0.0f,
            .wheel_speed_lf = 0.0f,
            .wheel_speed_rf = 0.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_6_expected{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 6: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_6.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_6.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_6.lm_torque_limit_positive, output_6_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_6.lm_torque_limit_positive, output_6_expected.lm_torque_limit_positive);
}

void test_7(const shared::util::Mapper<float>& pedal_to_torque, SimpVdInterface* simp_vd_int) {
    std::cout << "Test 7 loading" << std::endl;

    // Ensure traction control resets
    int time_ms = 200;

    VdOutput output_7 = simp_vd_int->update(
        VdInput{
            .driver_torque_request = 100.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 0.0f,
            .wheel_speed_rr = 0.0f,
            .wheel_speed_lf = 0.0f,
            .wheel_speed_rf = 0.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_7_expected{
        // Expected to calculate torque limit of 100.0f for both motors given test 2 input
        // Expected output of 50.0f after accounting for running avg of motor torque and test 6
        .lm_torque_limit_positive = 50.0f,
        .rm_torque_limit_positive = 50.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 7: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_7.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_7.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_7.lm_torque_limit_positive, output_7_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_7.lm_torque_limit_positive, output_7_expected.lm_torque_limit_positive);
}

int start_tests() {
    const float pedal_torque_lut_data[][2] = {
        {0.0f, 0.0f}, 
        {100.0f, 100.0f}
    };
    constexpr int pedal_torque_lut_length = (sizeof(pedal_torque_lut_data)) / (sizeof(pedal_torque_lut_data[0]));
    const shared::util::LookupTable<pedal_torque_lut_length> pedal_to_torque{pedal_torque_lut_data};

    SimpVdInterface simp_vd_int{pedal_to_torque};
    // Below tests all use the same SimpVdInterface object with running avg
    test_1(pedal_to_torque, &simp_vd_int);
    test_2(pedal_to_torque, &simp_vd_int);
    test_3(pedal_to_torque, &simp_vd_int);
    test_4(pedal_to_torque, &simp_vd_int);
    test_5(pedal_to_torque, &simp_vd_int);

    SimpVdInterface simp_vd_int_2{pedal_to_torque};
    // New object with reset running avg
    test_6(pedal_to_torque, &simp_vd_int_2);
    test_7(pedal_to_torque, &simp_vd_int_2);

    std::cout << "Testing done!" << std::endl;

    return 0;
}