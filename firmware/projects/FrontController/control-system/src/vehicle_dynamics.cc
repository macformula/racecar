/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "control-system/vehicle_dynamics.hpp"

#include "control-system/vehicle_dynamics_calc.hpp"

using namespace ctrl;

VehicleDynamics::VehicleDynamics(
    const shared::util::Mapper<float>& pedal_to_torque, float target_slip)
    : pedal_to_torque(pedal_to_torque), target_slip(target_slip) {}

void VehicleDynamics::Init(int time_ms) {
    traction_control_.Init(time_ms);
}

VehicleDynamics::Output VehicleDynamics::Update(const Input& input,
                                                int time_ms) {
    Output output{
        .lm_torque_limit_positive = 0.0f,
        .rm_torque_limit_positive = 0.0f,
        .lm_torque_limit_negative = 0.0f,  // negative limit fields fixed at
        .rm_torque_limit_negative = 0.0f,  // 0 in simulink model
        .left_motor_speed_request = 1000,
        .right_motor_speed_request = 1000,
    };

    float actual_slip =
        CalculateActualSlip(input.wheel_speed_lr, input.wheel_speed_rr,
                            input.wheel_speed_lf, input.wheel_speed_rf);
    float tc_scale_factor =
        traction_control_.UpdateScaleFactor(actual_slip, target_slip, time_ms);

    TorqueVector torque_vector;
    if (input.tv_enable) {
        torque_vector = AdjustTorqueVectoring(input.steering_angle);
    } else {
        torque_vector = {.left = 1.0f, .right = 1.0f};
    }

    float motor_torque_request = torque_request_.Update(
        input.driver_torque_request, input.brake_pedal_postion);

    motor_torque_req_running_avg.LoadValue(
        pedal_to_torque.Evaluate(motor_torque_request * tc_scale_factor));
    float smoothed_torque_request = motor_torque_req_running_avg.GetValue();

    output.lm_torque_limit_positive =
        smoothed_torque_request * torque_vector.left;
    output.rm_torque_limit_positive =
        smoothed_torque_request * torque_vector.right;

    return output;
}