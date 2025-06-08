/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#include "control-system/vehicle_dynamics.hpp"

#include "../../tuning.hpp"
#include "control-system/vehicle_dynamics_calc.hpp"
#include "shared/util/lookup_table.hpp"

using namespace ctrl;

VehicleDynamics::VehicleDynamics(LUT::Data pedal_to_torque,
                                 tuning::Profile profile, float target_slip)
    : pedal_to_torque(pedal_to_torque), target_slip(target_slip) {}

void VehicleDynamics::Init(int time_ms) {
    traction_control_.Init(time_ms);
}

VehicleDynamics::Output VehicleDynamics::Update(const Input& input,
                                                int time_ms) {
    // negative limit fields fixed at 0 in simulink model
    AmkManagerBase::Request left{
        .speed_request = 1000.f,
        .torque_limit_positive = 0.0f,
        .torque_limit_negative = 0.0f,
    };
    AmkManagerBase::Request right{
        .speed_request = 1000.f,
        .torque_limit_positive = 0.0f,
        .torque_limit_negative = 0.0f,
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
        LUT::Evaluate(pedal_to_torque, motor_torque_request * tc_scale_factor));
    float smoothed_torque_request = motor_torque_req_running_avg.GetValue();

    left.torque_limit_positive = smoothed_torque_request * torque_vector.left;
    right.torque_limit_positive = smoothed_torque_request * torque_vector.right;

    return Output{
        .left_motor_request = left,
        .right_motor_request = right,
    };
}