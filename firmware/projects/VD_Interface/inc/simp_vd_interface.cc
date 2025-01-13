/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "simp_vd_interface.hpp"
#include "shared/controls/motor_torque.h"

using namespace ctrl;

SimpVdInterface::SimpVdInterface(
    const shared::util::Mapper<float>& pedal_to_torque, float target_slip)
    : pedal_to_torque(pedal_to_torque), target_slip(target_slip) {}

VdOutput SimpVdInterface::update(const VdInput& input, int time_ms) {
    VdOutput output{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f, // negative limit fields set to
        .rm_torque_limit_negative = 0.0f, // 0 in simulink model
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000,
    };

    float actual_slip =
        CalculateActualSlip(input.wheel_speed_lr, input.wheel_speed_rr,
                            input.wheel_speed_lf, input.wheel_speed_rf);
    float tc_scale_factor = CalculateTCScaleFactor(actual_slip, target_slip, time_ms);

    float tv_factor_left;
    float tv_factor_right;

    float steering_angle = input.tv_enable ? input.steering_angle : 0.0f;
    TorqueVector<float> torque_vector = AdjustTorqueVectoring(steering_angle);

    float motor_torque_request = ComputeTorqueRequest(input.driver_torque_request,
                                                input.brake_pedal_postion);

    // Running avg calculation done within CalculateMotorTorque
    MotorTorque<float> motor_torque = CalculateMotorTorque(
        pedal_to_torque.Evaluate(motor_torque_request * tc_scale_factor),
        torque_vector);

    output.lm_torque_limit_positive = motor_torque.left_motor_torque_limit;
    output.rm_torque_limit_positive = motor_torque.right_motor_torque_limit;

    return output;
}