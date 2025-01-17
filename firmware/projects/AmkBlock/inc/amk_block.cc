#include "amk_block.hpp"

AmkBlock::AmkBlock(AmkStates initial_amk_state)
    : amk_state_(initial_amk_state) {}

AmkOutput AmkBlock::update(const AmkInput& input, const int time_ms) {
    using namespace generated::can;

    auto left =
        UpdateMotor<AMK0_ActualValues1, AMK0_ActualValues2, AMK0_SetPoints1>(
            input.left_actual1, input.left_actual2, input.left_motor_input,
            input.cmd);
    auto right =
        UpdateMotor<AMK1_ActualValues1, AMK1_ActualValues2, AMK1_SetPoints1>(
            input.right_actual1, input.right_actual2, input.right_motor_input,
            input.cmd);

    AmkOutput output{
        .status = ProcessOutputStatus(left.status, right.status),
        .left_setpoints = left.setpoint,
        .right_setpoints = right.setpoint,
        .inverter_enable = left.inverter_enable && right.inverter_enable};

    return output;
}

MiStatus AmkBlock::ProcessOutputStatus(MiStatus left_status,
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