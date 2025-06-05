#pragma once

#include "generated/can/veh_messages.hpp"

namespace driver_interface {

using State = generated::can::TxFC_Status::DiStatus_t;

enum class Command {
    INIT,
    HV_IS_ON,
    READY_TO_DRIVE,
    SHUTDOWN,
    RUN_ERROR,
    ERR_RESET,
};

}  // namespace driver_interface

class DriverInterface {
    using DashState = generated::can::RxDashboardStatus::DashState_t;

public:
    struct Input {
        driver_interface::Command command;
        float brake_pedal_pos;
        DashState dash_cmd;
        float accel_pedal_pos1;
        float accel_pedal_pos2;
    };
    struct Output {
        driver_interface::State status;
        float driver_torque_req;
        bool driver_speaker;
        bool brake_light_en;
        bool ready_to_drive;
    };

    Output Update(const Input input, int time_ms);
    DriverInterface() : status_(driver_interface::State::IDLE) {}

private:
    const float kBrakePressedThreshold = 10;

    driver_interface::State Transition(const Input input, bool di_error,
                                       const int time_ms);

    driver_interface::State status_;
    int speaker_start_time_;
};
