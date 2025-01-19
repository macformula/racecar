#pragma once

#include "shared/controls/driver_interface_fsm.hpp"

struct DiInput {
    DiCmd di_cmd;
    float rear_brake_pedal_position;
    float front_brake_pedal_position;
    bool driver_button;
    float accel_pedal_pos1;
    float accel_pedal_pos2;
    float steering_angle;
};

struct DiOutput {
    DiSts di_sts;
    float driver_torque_req;
    float steering_angle;
    float brake_pedal_position;
    bool driver_speaker;
    bool brake_light_en;
};

// Todo: check if this is needed or not
struct DriverInterfaceOutput {
    bool ready_to_drive;
    bool driver_speaker;
    float brake_pedal_position;
};

class DriverInterface {
public:
    DriverInterface();
    DiOutput Update(DiInput input, int time_ms);

private:
    // Add private variables if necessary. Prefer local variable in `Update()`
};