#pragma once

enum class DiCmd {
    GOV_E_DICMD = 8
};

enum class DiSts {
    GOV_E_DISTS = 7
};

// The simulink block has multiple inputs and outputs. We use `struct` to bind
// them all together. Define the input and output structs

struct DiInput {
    DiCmd di_cmd;
    float brake_pedal_position;
    bool driver_button;
    float steering_angle;
    float accel_pedal_pos1;
    float accel_pedal_pos2;
};

struct DiOutput {
    float driver_torque_req;
    float steering_angle;
    float brake_pedal_position;
    bool driver_speaker;
    bool brake_light_en;
    // fill this out
};

struct DriverInterfaceOutput {
    bool ready_to_drive;
    bool driver_speaker;
    float brake_pedal_position =
        std::max(input.front_brake_pressure, input.rear_brake_pressure);
}

class DriverInterface {
public:
    // Do not change the public interface
    DiOutput Update(DiInput input, int time_ms);

private:
    // Add private variables if necessary. Prefer local variable in `Update()`
};