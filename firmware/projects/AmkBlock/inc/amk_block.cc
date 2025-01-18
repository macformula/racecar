#include "amk_block.hpp"

AmkBlock::AmkBlock(AmkStates initial_amk_state)
    : amk_state_(initial_amk_state) {}

AmkOutput AmkBlock::update(const AmkInput& input, const int time_ms) {
    using namespace generated::can;

    auto left =
        UpdateMotor<AMK0_ActualValues1, AMK0_ActualValues2, AMK0_SetPoints1>(
            input.left_actual1, input.left_actual2, input.left_motor_input,
            input.cmd, time_ms, previous_state_output_.left_setpoints);
    auto right =
        UpdateMotor<AMK1_ActualValues1, AMK1_ActualValues2, AMK1_SetPoints1>(
            input.right_actual1, input.right_actual2, input.right_motor_input,
            input.cmd, time_ms, previous_state_output_.right_setpoints);

    previous_state_output_ = {
        .status = ProcessOutputStatus(left.status, right.status),
        .left_setpoints = left.setpoints,
        .right_setpoints = right.setpoints,
        .inverter_enable = left.inverter_enable && right.inverter_enable};

    return previous_state_output_;
}

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