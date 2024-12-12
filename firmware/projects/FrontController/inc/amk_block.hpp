#pragma once

#include "../generated/can/pt_can_messages.hpp"

enum class MiStatus {
    OFF,
    STARTUP,
    READY,
    RUNNING,
    MI_STS_ERROR
};

enum class MiCmd {
    CMD_STARTUP,
    CMD_SHUTDOWN,
    ERR_RESET
};

struct AmkInput {
    MiCmd cmd;
    AMK0_ActualValues1 amk_actual_values_1_left;
    AMK0_ActualValues2 amk_actual_values_2_left;
    AMK1_ActualValues1 amk_actual_values_1_right;
    AMK1_ActualValues2 amk_actual_values_2_right;
    double vd_left_motor_speed_request;
    double vd_left_motor_torque_limit_positive;
    double vd_left_motor_torque_limit_negative;
    double vd_right_motor_speed_request;
    double vd_right_motor_torque_limit_positive;
    double vd_right_motor_torque_limit_negative;
};

struct AmkOutput {
    MiStatus status;
    AMK1_SetPoints1 amk_setpoints_1_right;
    AMK0_SetPoints1 amk_setpoints_1_left;
    bool inverter;
};

// Only DECLARE the class methods an variables here. Write the method
// definitions in the .cc file.
class AmkBlock {
public:
    // do not change the public interface
    AmkBlock();
    AmkOutput update(const AmkInput& input, int time_ms);

private:
    // add private methods / variables as necessary.
};