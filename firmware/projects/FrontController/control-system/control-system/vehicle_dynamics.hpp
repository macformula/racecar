/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include <cstdint>

#include "../../tuning.hpp"
#include "motor_interface.hpp"
#include "shared/util/lookup_table.hpp"
#include "shared/util/moving_average.hpp"
#include "vehicle_dynamics_calc.hpp"

class VehicleDynamics {
    using LUT = shared::util::LookupTable<float>;

public:
    struct Input {
        float driver_torque_request;
        float brake_pedal_postion;
        float steering_angle;
        float wheel_speed_lr;
        float wheel_speed_rr;
        float wheel_speed_lf;
        float wheel_speed_rf;
        bool tv_enable;
    };
    struct Output {  // this should produce a left and right
                     // AmkManagerBase::MotorRequest
        AmkManagerBase::Request left_motor_request;
        AmkManagerBase::Request right_motor_request;
    };

    VehicleDynamics(
        LUT::Data pedal_to_torque, tuning::Profile profile,
        float target_slip = 0.2f);  // default target slip is float 0.2
    void Init(int time_ms);
    Output Update(const Input& input, int time_ms);

private:
    LUT::Data pedal_to_torque;
    // "Raw torque running avg" in Simulink
    shared::util::MovingAverage<10> motor_torque_req_running_avg;

    float target_slip;

    ctrl::TractionControl traction_control_;
    ctrl::TorqueRequest torque_request_;
};