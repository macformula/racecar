#pragma once

#include "../enums.hpp"
#include "../generated/can/veh_messages.hpp"

class DriverInterface {
    using DashState = generated::can::RxDashboardStatus::DashState_t;

public:
    struct Input {
        DiCmd command;
        float brake_pedal_pos;
        DashState dash_cmd;
        float accel_pedal_pos1;
        float accel_pedal_pos2;
    };
    struct Output {
        DiSts status;
        float driver_torque_req;
        bool driver_speaker;
        bool brake_light_en;
        bool ready_to_drive;
    };

    Output Update(const Input input, int time_ms);
    DriverInterface() : status_(DiSts::IDLE) {}

private:
    const float kBrakePressedThreshold = 10;

    DiSts Transition(const Input input, bool di_error, const int time_ms);

    DiSts status_;
    int speaker_start_time_;
};