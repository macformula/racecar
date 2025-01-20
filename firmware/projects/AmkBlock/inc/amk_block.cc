#include "amk_block.hpp"

MotorInterface::MotorInterface(AmkStates initial_amk_state) {
    left_amk_manager =
        AmkManager<generated::can::AMK0_SetPoints1>(initial_amk_state);
    right_amk_manager =
        AmkManager<generated::can::AMK1_SetPoints1>(initial_amk_state);
}

// Given motor inputs, will produce a motor output returned
AmkOutput MotorInterface::Update(const AmkInput& input, const int time_ms) {
    using namespace generated::can;

    auto left = left_amk_manager.UpdateMotor<AMK0_ActualValues1>(
        input.left_actual1, input.left_motor_input, input.cmd, time_ms);
    auto right = right_amk_manager.UpdateMotor<AMK1_ActualValues1>(
        input.right_actual1, input.right_motor_input, input.cmd, time_ms);

    return AmkOutput{
        .status = ProcessOutputStatus(left.status, right.status),
        .left_setpoints = left.setpoints,
        .right_setpoints = right.setpoints,
        .inverter_enable = left.inverter_enable && right.inverter_enable};

    return AmkOutput{};
}

// Processes the new output status given the left/right statuses
MiStatus MotorInterface::ProcessOutputStatus(MiStatus left_status,
                                             MiStatus right_status) {
    MiStatus output_status;
    if (left_status == MiStatus::ERROR || right_status == MiStatus::ERROR) {
        output_status = MiStatus::ERROR;
    } else if (left_status == right_status) {
        output_status = left_status;
    } else {
        output_status = previous_state_status_;
    }

    previous_state_status_ = output_status;
    return output_status;
}