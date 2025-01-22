/// @author Teghveer Singh Ateliey
/// @date 2024-11-23

#pragma once

#include <cstdint>

#include "shared/controls/motor_torque.h"
#include "shared/controls/tc_scale_factor.h"
#include "shared/controls/tvFactor.h"
#include "shared/util/mappers/lookup_table.hpp"
#include "shared/util/mappers/mapper.hpp"

struct VdInput {
    float driver_torque_request;
    float brake_pedal_postion;
    float steering_angle;
    float wheel_speed_lr;
    float wheel_speed_rr;
    float wheel_speed_lf;
    float wheel_speed_rf;
    bool tv_enable;
};

struct VdOutput {
    float lm_torque_limit_positive;
    float rm_torque_limit_positive;
    float lm_torque_limit_negative;
    float rm_torque_limit_negative;
    uint16_t left_motor_speed_request;
    uint16_t right_motor_speed_request;
};

class SimpVdInterface {
public:
    SimpVdInterface(
        const shared::util::Mapper<float>& pedal_to_torque,
        float target_slip = 0.2f);  // default target slip is float 0.2
    VdOutput update(const VdInput& input, int time_ms);

private:
    const shared::util::Mapper<float>& pedal_to_torque;
    shared::util::MovingAverage<float, 10>
        motor_torque_req_running_avg;  // "Raw torque running avg" in Simulink
                                       // model
    float target_slip;
};