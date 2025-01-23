#pragma once

#include "shared/controls/driver_interface_fsm.hpp"

class DriverInterface {
public:
    struct Input {
        DiCmd di_cmd;
        float brake_pedal_pos;
        bool driver_button;
        float accel_pedal_pos1;
        float accel_pedal_pos2;
        float steering_angle;
    };
    struct Output {
        DiSts di_sts;
        float driver_torque_req;
        float steering_angle;
        float brake_pedal_position;
        bool driver_speaker;
        bool brake_light_en;
    };

    Output Update(Input input, int time_ms);

private:
    DiFsm di_fsm;
};