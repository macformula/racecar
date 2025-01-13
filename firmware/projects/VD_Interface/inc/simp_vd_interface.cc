/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "simp_vd_interface.hpp"

using namespace ctrl;

SimpVdInterface::SimpVdInterface(float target_slip)
    : target_slip(target_slip) {}

VdOutput SimpVdInterface::update(const VdInput& input, int time_ms) {
    VdOutput output{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f, // negative limit fields set to
        .rm_torque_limit_negative = 0.0f, // 0 in simulink model
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000,
    };

    const float pedal_torque_lut_data[][2] = {
        {0.0f, 0.0f}, 
        {100.0f, 100.0f}
    };

    float actual_slip =
        CalculateActualSlip(input.wheel_speed_lr, input.wheel_speed_rr,
                            input.wheel_speed_lf, input.wheel_speed_rf);
    float tc_scale_factor = CalculateTCScaleFactor(actual_slip, target_slip, time_ms);

    float tv_factor_left;
    float tv_factor_right;

    float steering_angle = input.tv_enable ? input.steering_angle : 0.0f;
    std::tie(tv_factor_left, tv_factor_right) = AdjustTorqueVectoring(
        steering_angle, CreateTorqueVectoringFactor(steering_angle));

    constexpr int pedal_torque_lut_length = (sizeof(pedal_torque_lut_data)) / (sizeof(pedal_torque_lut_data[0]));
    const shared::util::LookupTable<pedal_torque_lut_length> pedal_to_torque{pedal_torque_lut_data};

    float motor_torque_request = ComputeTorqueRequest(input.driver_torque_request,
                                                input.brake_pedal_postion);

    // Running avg calculation done within CalculateMotorTorque
    std::tie(output.lm_torque_limit_positive, output.rm_torque_limit_positive) =
        CalculateMotorTorque(pedal_to_torque.Evaluate(motor_torque_request * tc_scale_factor),
                             tv_factor_right, tv_factor_left);

    return output;
}