/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "simp_vd_interface.hpp"
#include "shared/controls/testing.h"

#include <cassert>
#include <iostream>

void test_1(const shared::util::Mapper<float>& pedal_to_torque) {
    SimpVdInterface simp_vd_int{pedal_to_torque};
    int time_ms = 0;

    VdOutput output_1 = simp_vd_int.update(
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

void test_2(const shared::util::Mapper<float>& pedal_to_torque) {
    SimpVdInterface simp_vd_int{pedal_to_torque};
    int time_ms = 0;

    VdOutput output_2 = simp_vd_int.update(
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
        .lm_torque_limit_positive = 100.0f,
        .rm_torque_limit_positive = 100.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 1: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_2.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_2.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_2.lm_torque_limit_positive, output_2_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_2.lm_torque_limit_positive, output_2_expected.lm_torque_limit_positive);
}

void test_3(const shared::util::Mapper<float>& pedal_to_torque) {
    SimpVdInterface simp_vd_int{pedal_to_torque};
    int time_ms = 0;

    VdOutput output_3 = simp_vd_int.update(
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
        .lm_torque_limit_positive = 100.0f,
        .rm_torque_limit_positive = 68.3f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output 1: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_3.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_3.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_3.lm_torque_limit_positive, output_3_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_3.lm_torque_limit_positive, output_3_expected.lm_torque_limit_positive);
}

void test_4(const shared::util::Mapper<float>& pedal_to_torque) {
    SimpVdInterface simp_vd_int{pedal_to_torque};
    int time_ms = 0;

    VdOutput output_4 = simp_vd_int.update(
        VdInput{
            .driver_torque_request = 50.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 10.0f, // actual slip 0.5
            .wheel_speed_rr = 10.0f,
            .wheel_speed_lf = 15.0f,
            .wheel_speed_rf = 15.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_4_expected{
        .lm_torque_limit_positive = 50.0f,
        .rm_torque_limit_positive = 50.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output TC: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_4.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_4.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_4.lm_torque_limit_positive, output_4_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_4.rm_torque_limit_positive, output_4_expected.rm_torque_limit_positive);
}

void test_5(const shared::util::Mapper<float>& pedal_to_torque) {
    SimpVdInterface simp_vd_int{pedal_to_torque};
    int time_ms = 50;

    VdOutput output_5 = simp_vd_int.update(
        VdInput{
            .driver_torque_request = 50.0f,
            .brake_pedal_postion = 0.0f,
            .steering_angle = 0.0f,
            .wheel_speed_lr = 10.0f, // actual slip 0.5
            .wheel_speed_rr = 10.0f,
            .wheel_speed_lf = 15.0f,
            .wheel_speed_rf = 15.0f,
            .tv_enable = true
        },
        time_ms);

    VdOutput output_5_expected{
        .lm_torque_limit_positive = 0.0f, // tc scale factor is 0
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f,
        .rm_torque_limit_negative = 0.0f,
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000
    };

    std::cout << "Output TC: (negative torque limits and speed requests are always constant)" << std::endl;
    std::cout << "lm_torque_limit_positive: " << output_5.lm_torque_limit_positive << std::endl;
    std::cout << "rm_torque_limit_positive: " << output_5.rm_torque_limit_positive << std::endl;

    ASSERT_CLOSE(output_5.lm_torque_limit_positive, output_5_expected.lm_torque_limit_positive);
    ASSERT_CLOSE(output_5.rm_torque_limit_positive, output_5_expected.rm_torque_limit_positive);
}

int start_tests() {
    const float pedal_torque_lut_data[][2] = {
        {0.0f, 0.0f}, 
        {100.0f, 100.0f}
    };
    constexpr int pedal_torque_lut_length = (sizeof(pedal_torque_lut_data)) / (sizeof(pedal_torque_lut_data[0]));
    const shared::util::LookupTable<pedal_torque_lut_length> pedal_to_torque{pedal_torque_lut_data};

    test_1(pedal_to_torque);
    test_2(pedal_to_torque);
    test_3(pedal_to_torque);
    test_4(pedal_to_torque);
    test_5(pedal_to_torque);

    std::cout << "Testing done!" << std::endl;

    return 0;
}