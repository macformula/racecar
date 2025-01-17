#include "amk_block.hpp"

AmkBlock::AmkBlock(AmkStates initial_amk_state)
    : amk_state(initial_amk_state) {}

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

template <AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
UpdateMotorOutput<SP> AmkBlock::UpdateMotor(const V1 val1, const V2 val2,
                                            const MotorInput motor_input,
                                            const MiCmd cmd) {
    UpdateMotorOutput<SP> update_motor_output;
    SP setpoint;

    switch (amk_state) {
        case AmkStates::MOTOR_OFF_WAITING_FOR_GOV:
            update_motor_output.status = MiStatus::OFF;
            update_motor_output.inverter_enable = 0;

            setpoint.amk_b_inverter_on = 0;
            setpoint.amk_b_dc_on = 0;
            setpoint.amk_b_enable = 0;
            setpoint.amk_b_error_reset = 0;
            setpoint.amk__target_velocity = 0;
            setpoint.amk__torque_limit_positiv = 0;
            setpoint.amk__torque_limit_negativ = 0;

            if (cmd == MiCmd::STARTUP) {
                amk_state = AmkStates::STARTUP;
            }

            break;

        case AmkStates::STARTUP:

            if (val1.amk_b_error) {
                amk_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_inverter_on) {
                amk_state = AmkStates::READY;
            }

            break;

        case AmkStates::READY:
            update_motor_output.status = MiStatus::READY;
            update_motor_output.inverter_enable = 0;

            if (val1.amk_b_error) {
                amk_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_quit_inverter_on) {
                amk_state = AmkStates::RUNNING;
            }

            break;

        case AmkStates::RUNNING:
            update_motor_output.status = MiStatus::RUNNING;

            setpoint.amk__target_velocity = motor_input.speed_request;
            setpoint.amk__torque_limit_positiv =
                motor_input.torque_limit_positive;
            setpoint.amk__torque_limit_negativ =
                motor_input.torque_limit_negative;

            if (val1.amk_b_error) {
                amk_state = AmkStates::ERROR_DETECTED;
            } else if (cmd == MiCmd::SHUTDOWN) {
                amk_state = AmkStates::SHUTDOWN;
            }

            break;

        case AmkStates::SHUTDOWN:
            update_motor_output.inverter_enable = 0;

            setpoint.amk__target_velocity = 0;
            setpoint.amk__torque_limit_positiv = 0;
            setpoint.amk__torque_limit_negativ = 0;

            break;

        case AmkStates::ERROR_DETECTED:
            update_motor_output.status = MiStatus::ERROR;

            if (cmd == MiCmd::ERR_RESET) {
                amk_state = AmkStates::ERROR_RESET;
            }

            break;

        case AmkStates::ERROR_RESET:

            if (val1.amk_b_system_ready) {
                amk_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV;
            }

            break;
    }

    return update_motor_output;
}

MiStatus AmkBlock::ProcessOutputStatus(MiStatus left_status,
                                       MiStatus right_status) {
    MiStatus output_status;
    if (left_status == MiStatus::ERROR || right_status == MiStatus::ERROR) {
        output_status = MiStatus::ERROR;
    } else if (left_status == right_status) {
        output_status = left_status;
    } else {
        output_status = previous_state_status;
    }

    previous_state_status = output_status;
    return output_status;
}