#pragma once

#include <concepts>

#include "generated/can/pt_can_messages.hpp"

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

// MotorInput struct used in AmkInput to group identical left/right values
struct MotorInput {
    double speed_request;
    double torque_limit_positive;
    double torque_limit_negative;
};

// AmkInput struct, the input that is fed into the state machine
struct AmkInput {
    MiCmd cmd;
    generated::can::AMK0_ActualValues1 left_actual1;
    generated::can::AMK1_ActualValues1 right_actual1;
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

// SetPoints concept, combines AMK0_SetPoints1 and AMK1_SetPoints1 into one
// common type for easier use
template <typename MsgType>
concept SetPoints = requires(MsgType msg) {
    { msg.amk_b_inverter_on } -> std::convertible_to<bool>;
    { msg.amk_b_dc_on } -> std::convertible_to<bool>;
    { msg.amk_b_enable } -> std::convertible_to<bool>;
    { msg.amk_b_error_reset } -> std::convertible_to<bool>;
    { msg.amk__target_velocity } -> std::convertible_to<int16_t>;
    { msg.amk__torque_limit_positiv } -> std::convertible_to<int16_t>;
    { msg.amk__torque_limit_negativ } -> std::convertible_to<int16_t>;
};

// UpdateMotorOutput struct, used to create a return type for UpdateMotor such
// that it does not matter if setpoints is for left or right
template <SetPoints SP>
struct UpdateMotorOutput {
    SP setpoints;
    MiStatus status;
    bool inverter_enable;
};

template <AmkActualValues1 V1, SetPoints SP>
class AmkManager {
public:
    AmkManager(
        AmkStates initial_amk_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV);

    UpdateMotorOutput<SP> UpdateMotor(const V1 val1,
                                      const MotorInput motor_input,
                                      const MiCmd cmd, int time_ms);

    AmkStates Transition(const V1 val1, const MiCmd cmd, const int time_ms);

private:
    AmkStates amk_state_;
    int amk_state_start_time_ = 0;
    UpdateMotorOutput<SP> output_{.status = MiStatus::OFF,
                                  .inverter_enable = false};
};

class MotorInterface {
public:
    AmkOutput Update(const AmkInput& input, const int time_ms);

private:
    AmkManager<generated::can::AMK0_ActualValues1,
               generated::can::AMK0_SetPoints1>
        left_amk_manager{AmkStates::MOTOR_OFF_WAITING_FOR_GOV};
    AmkManager<generated::can::AMK1_ActualValues1,
               generated::can::AMK1_SetPoints1>
        right_amk_manager{AmkStates::MOTOR_OFF_WAITING_FOR_GOV};
    MiStatus status_ = MiStatus::OFF;

    MiStatus ProcessOutputStatus(MiStatus left_status, MiStatus right_status);
};

template <AmkActualValues1 V1, SetPoints SP>
AmkManager<V1, SP>::AmkManager(AmkStates initial_amk_state)
    : amk_state_(initial_amk_state) {}

template <AmkActualValues1 V1, SetPoints SP>
UpdateMotorOutput<SP> AmkManager<V1, SP>::UpdateMotor(
    const V1 val1, const MotorInput motor_input, const MiCmd cmd,
    const int time_ms) {
    using enum AmkStates;

    amk_state_ = Transition(val1, cmd, time_ms);
    switch (amk_state_) {
        case MOTOR_OFF_WAITING_FOR_GOV:
            output_.status = MiStatus::OFF;
            output_.inverter_enable = false;

            output_.setpoints.amk_b_inverter_on = false;
            output_.setpoints.amk_b_dc_on = false;
            output_.setpoints.amk_b_enable = false;
            output_.setpoints.amk_b_error_reset = false;
            output_.setpoints.amk__target_velocity = 0;
            output_.setpoints.amk__torque_limit_positiv = 0;
            output_.setpoints.amk__torque_limit_negativ = 0;

            break;

        case STARTUP_SYS_READY:
            output_.status = MiStatus::STARTUP;

            break;

        case STARTUP_TOGGLE_D_CON:
            output_.setpoints.amk_b_dc_on = true;

            break;

        case STARTUP_ENFORCE_SETPOINTS_ZERO:
            output_.setpoints.amk__target_velocity = 0;
            output_.setpoints.amk__torque_limit_positiv = 0;
            output_.setpoints.amk__torque_limit_negativ = 0;

            break;

        case STARTUP_COMMAND_ON:
            output_.setpoints.amk_b_enable = true;
            output_.setpoints.amk_b_inverter_on = true;

            break;

        case READY:
            output_.status = MiStatus::READY;
            output_.inverter_enable = true;

            break;

        case RUNNING:
            output_.status = MiStatus::RUNNING;

            output_.setpoints.amk__target_velocity = motor_input.speed_request;
            output_.setpoints.amk__torque_limit_positiv =
                motor_input.torque_limit_positive;
            output_.setpoints.amk__torque_limit_negativ =
                motor_input.torque_limit_negative;

            break;

        case SHUTDOWN:
            output_.inverter_enable = false;

            output_.setpoints.amk__target_velocity = 0;
            output_.setpoints.amk__torque_limit_positiv = 0;
            output_.setpoints.amk__torque_limit_negativ = 0;

            break;

        case ERROR_DETECTED:
            output_.status = MiStatus::ERROR;

            break;

        case ERROR_RESET_ENFORCE_SETPOINTS_ZERO:
            output_.setpoints.amk__target_velocity = 0;
            output_.setpoints.amk__torque_limit_positiv = 0;
            output_.setpoints.amk__torque_limit_negativ = 0;
            output_.setpoints.amk_b_inverter_on = false;

            break;

        case ERROR_RESET_TOGGLE_ENABLE:
            output_.setpoints.amk_b_enable = false;

            break;

        case ERROR_RESET_SEND_RESET:
            output_.setpoints.amk_b_error_reset = true;

            break;

        case ERROR_RESET_TOGGLE_RESET:
            output_.setpoints.amk_b_error_reset = false;

            break;
    }

    return output_;
}

// Computes the state to transition to in an update
template <AmkActualValues1 V1, SetPoints SP>
AmkStates AmkManager<V1, SP>::Transition(const V1 val1, const MiCmd cmd,
                                         const int time_ms) {
    using enum AmkStates;

    // If any of these states have amk_b_error set, move to ERROR_DETECTED state
    if ((amk_state_ == STARTUP_SYS_READY ||
         amk_state_ == STARTUP_TOGGLE_D_CON ||
         amk_state_ == STARTUP_ENFORCE_SETPOINTS_ZERO ||
         amk_state_ == STARTUP_COMMAND_ON || amk_state_ == READY ||
         amk_state_ == RUNNING) &&
        val1.amk_b_error) {
        amk_state_start_time_ = time_ms;
        return ERROR_DETECTED;
    }

    AmkStates new_state = amk_state_;
    int elapsed_time = time_ms - amk_state_start_time_;

    switch (amk_state_) {
        case MOTOR_OFF_WAITING_FOR_GOV:
            if (cmd == MiCmd::STARTUP) {
                new_state = STARTUP_SYS_READY;
            }

            break;

        case STARTUP_SYS_READY:
            if (val1.amk_b_system_ready) {
                new_state = STARTUP_TOGGLE_D_CON;
            }

            break;

        case STARTUP_TOGGLE_D_CON:
            if (val1.amk_b_dc_on && val1.amk_b_quit_dc_on) {
                new_state = STARTUP_ENFORCE_SETPOINTS_ZERO;
            }

            break;

        case STARTUP_ENFORCE_SETPOINTS_ZERO:
            if (elapsed_time >= 100) {
                new_state = STARTUP_COMMAND_ON;
            }

            break;

        case STARTUP_COMMAND_ON:
            if (val1.amk_b_inverter_on) {
                new_state = READY;
            }

            break;

        case READY:
            if (val1.amk_b_quit_inverter_on) {
                new_state = RUNNING;
            }

            break;

        case RUNNING:
            if (cmd == MiCmd::SHUTDOWN) {
                new_state = SHUTDOWN;
            }

            break;

        case SHUTDOWN:
            if (elapsed_time >= 500) {
                new_state = MOTOR_OFF_WAITING_FOR_GOV;
            }

            break;

        case ERROR_DETECTED:
            if (cmd == MiCmd::ERR_RESET) {
                new_state = ERROR_RESET_ENFORCE_SETPOINTS_ZERO;
            }

            break;

        case ERROR_RESET_ENFORCE_SETPOINTS_ZERO:
            if (!val1.amk_b_inverter_on) {
                new_state = ERROR_RESET_TOGGLE_ENABLE;
            }

            break;

        case ERROR_RESET_TOGGLE_ENABLE:
            if (elapsed_time >= 500) {
                new_state = ERROR_RESET_SEND_RESET;
            }

            break;

        case ERROR_RESET_SEND_RESET:
            if (elapsed_time >= 500) {
                new_state = ERROR_RESET_TOGGLE_RESET;
            }

            break;

        case ERROR_RESET_TOGGLE_RESET:
            if (val1.amk_b_system_ready) {
                new_state = MOTOR_OFF_WAITING_FOR_GOV;
            }

            break;
    }

    // If a new state has been entered, set the start time of the current state
    if (new_state != amk_state_) {
        amk_state_start_time_ = time_ms;
    }

    return new_state;
}