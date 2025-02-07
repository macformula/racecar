#include "control-system/amk_block.hpp"

#include "control-system/enums.hpp"

template <AmkActualValues1 LV1, AmkActualValues1 RV1, SetPoints LSP,
          SetPoints RSP>
MotorInterface<LV1, RV1, LSP, RSP>::Output
MotorInterface<LV1, RV1, LSP, RSP>::Update(const Input& input,
                                           const int time_ms) {
    auto left = left_amk_manager.UpdateMotor(
        input.left_actual1, input.left_motor_input, input.cmd, time_ms);
    auto right = right_amk_manager.UpdateMotor(
        input.right_actual1, input.right_motor_input, input.cmd, time_ms);

    UpdateOutputStatus(left.status, right.status);

    return Output{
        .status = status_,
        .left_setpoints = left.setpoints,
        .right_setpoints = right.setpoints,
        .inverter_enable = left.inverter_enable && right.inverter_enable};
}

template <AmkActualValues1 LV1, AmkActualValues1 RV1, SetPoints LSP,
          SetPoints RSP>
void MotorInterface<LV1, RV1, LSP, RSP>::UpdateOutputStatus(
    MiSts left_status, MiSts right_status) {
    if (left_status == MiSts::ERR || right_status == MiSts::ERR) {
        status_ = MiSts::ERR;
    } else if (left_status == right_status) {
        status_ = left_status;
    }  // else keep previous status
}