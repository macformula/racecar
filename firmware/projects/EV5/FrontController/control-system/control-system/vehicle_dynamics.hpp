/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include <cstdint>

#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"
#include "vehicle_dynamics_calc.hpp"

class VehicleDynamics {
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
        float lm_torque_limit_positive;
        float rm_torque_limit_positive;
        float lm_torque_limit_negative;
        float rm_torque_limit_negative;
        uint16_t left_motor_speed_request;
        uint16_t right_motor_speed_request;
    };

    VehicleDynamics(
        const shared::util::Mapper<float>& pedal_to_torque,
        float target_slip = 0.2f);  // default target slip is float 0.2
    void Init(int time_ms);
    Output Update(const Input& input, int time_ms);

private:
    const shared::util::Mapper<float>& pedal_to_torque;
    shared::util::MovingAverage<float, 10>
        motor_torque_req_running_avg;  // "Raw torque running avg" in Simulink
                                       // model
    float target_slip;

    ctrl::TractionControl traction_control_;
    ctrl::TorqueRequest torque_request_;
};