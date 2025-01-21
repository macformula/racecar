#include "amk_block.hpp"

MotorInterface::MotorInterface() {}

// Given motor inputs, will produce a motor output returned
AmkOutput MotorInterface::Update(const AmkInput& input, const int time_ms) {
    auto left = left_amk_manager.UpdateMotor(
        input.left_actual1, input.left_motor_input, input.cmd, time_ms);
    auto right = right_amk_manager.UpdateMotor(
        input.right_actual1, input.right_motor_input, input.cmd, time_ms);

    return AmkOutput{
        .status = ProcessOutputStatus(left.status, right.status),
        .left_setpoints = left.setpoints,
        .right_setpoints = right.setpoints,
        .inverter_enable = left.inverter_enable && right.inverter_enable};
}

// Processes the new output status given the left/right statuses
MiStatus MotorInterface::ProcessOutputStatus(MiStatus left_status,
                                             MiStatus right_status) {
    if (left_status == MiStatus::ERROR || right_status == MiStatus::ERROR) {
        previous_state_status_ = MiStatus::ERROR;
    } else if (left_status == right_status) {
        previous_state_status_ = left_status;
    }

    return previous_state_status_;
}