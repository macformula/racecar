#include "amk_block.hpp"

AmkBlock::AmkBlock(AmkStates initial_amk_state)
    : left_amk_state_(initial_amk_state), right_amk_state_(initial_amk_state) {}

// Given motor inputs, will produce a motor output returned
AmkOutput AmkBlock::Update(const AmkInput& input, const int time_ms) {
    using namespace generated::can;

    UpdateMotorOutput<generated::can::AMK0_SetPoints1> left = {
        .setpoints = previous_state_output_.left_setpoints,
        .status = previous_state_output_.status,
        .inverter_enable = previous_state_output_.inverter_enable,
        .amk_state = left_amk_state_,
        .amk_state_start_time = left_amk_state_start_time_};
    UpdateMotor<AMK0_ActualValues1, AMK0_ActualValues2, AMK0_SetPoints1>(
        input.left_actual1, input.left_actual2, input.left_motor_input,
        input.cmd, time_ms, left);

    UpdateMotorOutput<generated::can::AMK1_SetPoints1> right = {
        .setpoints = previous_state_output_.right_setpoints,
        .status = previous_state_output_.status,
        .inverter_enable = previous_state_output_.inverter_enable,
        .amk_state = right_amk_state_,
        .amk_state_start_time = right_amk_state_start_time_};
    UpdateMotor<AMK1_ActualValues1, AMK1_ActualValues2, AMK1_SetPoints1>(
        input.right_actual1, input.right_actual2, input.right_motor_input,
        input.cmd, time_ms, right);

    left_amk_state_ = left.amk_state;
    left_amk_state_start_time_ = left.amk_state_start_time;
    right_amk_state_ = right.amk_state;
    right_amk_state_start_time_ = right.amk_state_start_time;
    previous_state_output_ = {
        .status = ProcessOutputStatus(left.status, right.status),
        .left_setpoints = left.setpoints,
        .right_setpoints = right.setpoints,
        .inverter_enable = left.inverter_enable && right.inverter_enable};

    return previous_state_output_;
}

// Processes the new output status given the left/right statuses
MiStatus AmkBlock::ProcessOutputStatus(MiStatus left_status,
                                       MiStatus right_status) {
    MiStatus output_status;
    if (left_status == MiStatus::ERROR || right_status == MiStatus::ERROR) {
        output_status = MiStatus::ERROR;
    } else if (left_status == right_status) {
        output_status = left_status;
    } else {
        output_status = previous_state_output_.status;
    }

    return output_status;
}