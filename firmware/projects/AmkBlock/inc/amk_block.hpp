#pragma once

#include <concepts>

#include "../generated/can/pt_can_messages.hpp"

// MiStatus enum class used in AmkOutput
enum class MiStatus {
    UNKNOWN,
    INIT,
    STARTUP,
    READY,
    RUNNING,
    SHUTDOWN,
    ERROR,
    OFF
};

// MiCmd enum class used in AmkInput
enum class MiCmd {
    INIT,
    IDLE,
    STARTUP,
    SHUTDOWN,
    ERR_RESET
};

// MotorInput struct used in AmkInput to group identical left/right values
struct MotorInput {
    double speed_request;
    double torque_limit_negative;
    double torque_limit_positive;
};

// AmkInput struct, the input that is fed into the state machine
struct AmkInput {
    MiCmd cmd;
    generated::can::AMK0_ActualValues1 left_actual1;
    generated::can::AMK0_ActualValues2 left_actual2;
    generated::can::AMK1_ActualValues1 right_actual1;
    generated::can::AMK1_ActualValues2 right_actual2;
    MotorInput left_motor_input;
    MotorInput right_motor_input;
};

// AmkOutput struct, the output that is produced from the state machine
struct AmkOutput {
    MiStatus status;
    generated::can::AMK0_SetPoints1 left_setpoints;
    generated::can::AMK1_SetPoints1 right_setpoints;
    bool inverter_enable;
};

// AmkActualValues1 concept, combines AMK0_ActualValues1 and AMK1_ActualValues1
// into one common type for easier use
template <typename MsgType>
concept AmkActualValues1 = requires(MsgType msg) {
    { msg.amk_b_system_ready } -> std::convertible_to<bool>;
    { msg.amk_b_error } -> std::convertible_to<bool>;
    { msg.amk_b_warn } -> std::convertible_to<bool>;
    { msg.amk_b_quit_dc_on } -> std::convertible_to<bool>;
    { msg.amk_b_dc_on } -> std::convertible_to<bool>;
    { msg.amk_b_quit_inverter_on } -> std::convertible_to<bool>;
    { msg.amk_b_inverter_on } -> std::convertible_to<bool>;
    { msg.amk_b_derating } -> std::convertible_to<bool>;
    { msg.amk__actual_velocity } -> std::convertible_to<int16_t>;
    { msg.amk__torque_current } -> std::convertible_to<int16_t>;
    { msg.amk__magnetizing_current } -> std::convertible_to<int16_t>;
};

// AmkActualValues2 concept, combines AMK0_ActualValues2 and AMK1_ActualValues2
// into one common type for easier use
template <typename MsgType>
concept AmkActualValues2 = requires(MsgType msg) {
    { msg.amk__temp_motor } -> std::convertible_to<float>;
    { msg.amk__temp_inverter } -> std::convertible_to<float>;
    { msg.amk__error_info } -> std::convertible_to<uint16_t>;
    { msg.amk__temp_igbt } -> std::convertible_to<float>;
};

// SetPoint concept, combines AMK0_SetPoints1 and AMK1_SetPoints1 into one
// common type for easier use
template <typename MsgType>
concept SetPoint = requires(MsgType msg) {
    { msg.amk_b_inverter_on } -> std::convertible_to<bool>;
    { msg.amk_b_dc_on } -> std::convertible_to<bool>;
    { msg.amk_b_enable } -> std::convertible_to<bool>;
    { msg.amk_b_error_reset } -> std::convertible_to<bool>;
    { msg.amk__target_velocity } -> std::convertible_to<int16_t>;
    { msg.amk__torque_limit_positiv } -> std::convertible_to<int16_t>;
    { msg.amk__torque_limit_negativ } -> std::convertible_to<int16_t>;
};

// UpdateMotorOutput struct, used to create a return type for UpdateMotor such
// that it does not matter if setpoint is for left or right
template <SetPoint SP>
struct UpdateMotorOutput {
    SP setpoint;
    MiStatus status;
    bool inverter_enable;
};

// AmkStates enum class which defines all states in the amk state machine
enum class AmkStates {
    MOTOR_OFF_WAITING_FOR_GOV,
    STARTUP_SYS_READY,
    STARTUP_TOGGLE_D_CON,
    STARTUP_ENFORCE_SETPOINTS_ZERO,
    STARTUP_COMMAND_ON,
    READY,
    RUNNING,
    SHUTDOWN,
    ERROR_DETECTED,
    ERROR_RESET_ENFORCE_SETPOINTS_ZERO,
    ERROR_RESET_TOGGLE_ENABLE,
    ERROR_RESET_SEND_RESET,
    ERROR_RESET_TOGGLE_RESET
};

class AmkBlock {
public:
    AmkBlock(
        AmkStates initial_amk_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV);

    AmkOutput update(const AmkInput& input, const int time_ms);

private:
    AmkStates amk_state_;
    int amk_state_start_time_ = 0;
    MiStatus previous_state_status_ = MiStatus::OFF;

    template <AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
    UpdateMotorOutput<SP> UpdateMotor(const V1 val1, const V2 val2,
                                      const MotorInput motor_input,
                                      const MiCmd cmd, int time_ms);
    template <AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
    AmkStates Transition(const V1 val1, const V2 val2,
                         const MotorInput motor_input, const MiCmd cmd,
                         int time_ms);
    MiStatus ProcessOutputStatus(MiStatus left_status, MiStatus right_status);
};

template <AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
UpdateMotorOutput<SP> AmkBlock::UpdateMotor(const V1 val1, const V2 val2,
                                            const MotorInput motor_input,
                                            const MiCmd cmd, int time_ms) {
    UpdateMotorOutput<SP> update_motor_output;
    SP setpoint;
    amk_state_ = Transition<V1, V2, SP>(val1, val2, motor_input, cmd, time_ms);

    switch (amk_state_) {
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

            break;

        case AmkStates::STARTUP_SYS_READY:
            update_motor_output.status = MiStatus::STARTUP;

            break;

        case AmkStates::STARTUP_TOGGLE_D_CON:
            setpoint.amk_b_dc_on = 1;

            break;

        case AmkStates::STARTUP_ENFORCE_SETPOINTS_ZERO:
            setpoint.amk__target_velocity = 0;
            setpoint.amk__torque_limit_positiv = 0;
            setpoint.amk__torque_limit_negativ = 0;

            break;

        case AmkStates::STARTUP_COMMAND_ON:
            setpoint.amk_b_enable = 1;
            setpoint.amk_b_inverter_on = 1;

            break;

        case AmkStates::READY:
            update_motor_output.status = MiStatus::READY;
            update_motor_output.inverter_enable = 0;

            break;

        case AmkStates::RUNNING:
            update_motor_output.status = MiStatus::RUNNING;

            setpoint.amk__target_velocity = motor_input.speed_request;
            setpoint.amk__torque_limit_positiv =
                motor_input.torque_limit_positive;
            setpoint.amk__torque_limit_negativ =
                motor_input.torque_limit_negative;

            break;

        case AmkStates::SHUTDOWN:
            update_motor_output.inverter_enable = 0;

            setpoint.amk__target_velocity = 0;
            setpoint.amk__torque_limit_positiv = 0;
            setpoint.amk__torque_limit_negativ = 0;

            break;

        case AmkStates::ERROR_DETECTED:
            update_motor_output.status = MiStatus::ERROR;

            break;

        case AmkStates::ERROR_RESET_ENFORCE_SETPOINTS_ZERO:
            setpoint.amk__target_velocity = 0;
            setpoint.amk__torque_limit_positiv = 0;
            setpoint.amk__torque_limit_negativ = 0;
            setpoint.amk_b_inverter_on = 0;

            break;

        case AmkStates::ERROR_RESET_TOGGLE_ENABLE:
            setpoint.amk_b_enable = 0;

            break;

        case AmkStates::ERROR_RESET_SEND_RESET:
            setpoint.amk_b_error_reset = 1;

            break;

        case AmkStates::ERROR_RESET_TOGGLE_RESET:
            setpoint.amk_b_error_reset = 0;

            break;
    }

    return update_motor_output;
}

template <AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
AmkStates AmkBlock::Transition(const V1 val1, const V2 val2,
                               const MotorInput motor_input, const MiCmd cmd,
                               int time_ms) {
    // Add group condition for common if conditions

    AmkStates new_state = amk_state_;
    int elapsed_time = time_ms - amk_state_start_time_;

    switch (amk_state_) {
        case AmkStates::MOTOR_OFF_WAITING_FOR_GOV:
            if (cmd == MiCmd::STARTUP) {
                new_state = AmkStates::STARTUP_SYS_READY;
            }

            break;

        case AmkStates::STARTUP_SYS_READY:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_system_ready) {
                new_state = AmkStates::STARTUP_TOGGLE_D_CON;
            }

            break;

        case AmkStates::STARTUP_TOGGLE_D_CON:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_dc_on && val1.amk_b_quit_dc_on) {
                new_state = AmkStates::STARTUP_ENFORCE_SETPOINTS_ZERO;
            }

            break;

        case AmkStates::STARTUP_ENFORCE_SETPOINTS_ZERO:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (elapsed_time >= 100) {
                new_state = AmkStates::STARTUP_COMMAND_ON;
            }

            break;

        case AmkStates::STARTUP_COMMAND_ON:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_inverter_on) {
                new_state = AmkStates::READY;
            }

            break;

        case AmkStates::READY:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (val1.amk_b_quit_inverter_on) {
                new_state = AmkStates::RUNNING;
            }

            break;

        case AmkStates::RUNNING:
            if (val1.amk_b_error) {
                new_state = AmkStates::ERROR_DETECTED;
            } else if (cmd == MiCmd::SHUTDOWN) {
                new_state = AmkStates::SHUTDOWN;
            }

            break;

        case AmkStates::SHUTDOWN:
            if (elapsed_time >= 500) {
                new_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV;
            }

            break;

        case AmkStates::ERROR_DETECTED:
            if (cmd == MiCmd::ERR_RESET) {
                new_state = AmkStates::ERROR_RESET_ENFORCE_SETPOINTS_ZERO;
            }

            break;

        case AmkStates::ERROR_RESET_ENFORCE_SETPOINTS_ZERO:
            if (val1.amk_b_inverter_on) {
                new_state = AmkStates::ERROR_RESET_TOGGLE_ENABLE;
            }

            break;

        case AmkStates::ERROR_RESET_TOGGLE_ENABLE:
            if (elapsed_time >= 500) {
                new_state = AmkStates::ERROR_RESET_SEND_RESET;
            }

            break;

        case AmkStates::ERROR_RESET_SEND_RESET:
            if (elapsed_time >= 500) {
                new_state = AmkStates::ERROR_RESET_TOGGLE_RESET;
            }

            break;

        case AmkStates::ERROR_RESET_TOGGLE_RESET:
            if (val1.amk_b_system_ready) {
                new_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV;
            }

            break;
    }

    // If a new state has been entered, set the start time of the current state
    if (new_state != amk_state_) {
        amk_state_start_time_ = time_ms;
    }

    return new_state;
}