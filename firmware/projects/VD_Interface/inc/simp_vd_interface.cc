/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "simp_vd_interface.hpp"

using namespace ctrl;

SimpVdInterface::SimpVdInterface(
    const shared::util::Mapper<float>& pedal_to_torque, float target_slip)
    : pedal_to_torque(pedal_to_torque), target_slip(target_slip) {}

VdOutput SimpVdInterface::update(const VdInput& input, int time_ms) {
    VdOutput output{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f, // negative limit fields fixed at
        .rm_torque_limit_negative = 0.0f, // 0 in simulink model
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000,
    };

    float actual_slip =
        CalculateActualSlip(input.wheel_speed_lr, input.wheel_speed_rr,
                            input.wheel_speed_lf, input.wheel_speed_rf);
    float tc_scale_factor = CalculateTCScaleFactor(actual_slip, target_slip, time_ms);

    TorqueVector<float> torque_vector;
    if (input.tv_enable) {
        torque_vector = AdjustTorqueVectoring(input.steering_angle);
    } else {
        torque_vector.left = 1.0f;
        torque_vector.right = 1.0f;
    }
    
    float motor_torque_request = ComputeTorqueRequest(input.driver_torque_request,
                                                input.brake_pedal_postion);

    running_average.LoadValue(pedal_to_torque.Evaluate(motor_torque_request * tc_scale_factor));
    float running_average_value = running_average.GetValue();

    output.lm_torque_limit_positive = running_average_value * torque_vector.left;
    output.rm_torque_limit_positive = running_average_value * torque_vector.right;

    return output;
}