#pragma once

#include "app.h"

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
    SimpVdInterface();
    VdOutput update(const VdInput& input, int time_ms);

private:
    
};