#pragma once
#include <concepts>
#include <cstdint>

#include "enums.hpp"

// AmkActualValues1 concept, combines AMK0_ActualValues1 and AMK1_ActualValues1
// into one common type for easier use
template <typename MsgType>
concept AmkActualValues1 = requires(MsgType msg) {
    { msg.AMK_bSystemReady() } -> std::same_as<bool>;
    { msg.AMK_bError() } -> std::same_as<bool>;
    { msg.AMK_bWarn() } -> std::same_as<bool>;
    { msg.AMK_bQuitDcOn() } -> std::same_as<bool>;
    { msg.AMK_bDcOn() } -> std::same_as<bool>;
    { msg.AMK_bQuitInverterOn() } -> std::same_as<bool>;
    { msg.AMK_bInverterOn() } -> std::same_as<bool>;
    { msg.AMK_bDerating() } -> std::same_as<bool>;
    { msg.AMK_ActualVelocity() } -> std::same_as<int16_t>;
    { msg.AMK_TorqueCurrent() } -> std::same_as<int16_t>;
    { msg.AMK_MagnetizingCurrent() } -> std::same_as<int16_t>;
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

class AmkManagerBase {
    // This exists solely to define types which are not dependent on the
    // AmkManager template
public:
    struct Request {
        float speed_request;
        float torque_limit_positive;
        float torque_limit_negative;
    };
    enum class FsmState {
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
};

template <AmkActualValues1 V1, SetPoints SP>
class AmkManager : public AmkManagerBase {
public:
    struct Output {
        SP setpoints;
        MiSts status;
        bool inverter_enable;
        AmkManagerBase::FsmState fsm_state;
    };

    Output UpdateMotor(const V1 val1, const Request motor_input,
                       const MiCmd cmd, int time_ms);

private:
    FsmState amk_state_;
    int amk_state_start_time_ = 0;
    Output output_{};

    FsmState Transition(const V1 val1, const MiCmd cmd, const int time_ms);
};

template <AmkActualValues1 LEFT_ACTUAL1, AmkActualValues1 RIGHT_ACTUAL1,
          SetPoints LEFT_SP, SetPoints RIGHT_SP>
class MotorInterface {
public:
    struct Input {
        MiCmd cmd;
        LEFT_ACTUAL1 left_actual1;
        RIGHT_ACTUAL1 right_actual1;
        AmkManagerBase::Request left_motor_input;
        AmkManagerBase::Request right_motor_input;
    };

    struct Output {
        MiSts status;
        LEFT_SP left_setpoints;
        RIGHT_SP right_setpoints;
        bool inverter_enable;
    };

    Output Update(const Input& input, const int time_ms);

private:
    AmkManager<LEFT_ACTUAL1, LEFT_SP> left_amk_manager{};
    AmkManager<RIGHT_ACTUAL1, RIGHT_SP> right_amk_manager{};
    MiSts status_ = MiSts::OFF;

    void UpdateOutputStatus(MiSts left_status, MiSts right_status);
};

template <AmkActualValues1 V1, SetPoints SP>
AmkManager<V1, SP>::Output AmkManager<V1, SP>::UpdateMotor(
    const V1 val1, const Request motor_input, const MiCmd cmd,
    const int time_ms) {
    using enum FsmState;

    amk_state_ = Transition(val1, cmd, time_ms);
    output_.fsm_state = amk_state_;
    switch (amk_state_) {
        case MOTOR_OFF_WAITING_FOR_GOV:
            output_.status = MiSts::OFF;
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
            output_.status = MiSts::STARTUP;

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
            output_.status = MiSts::READY;
            output_.inverter_enable = true;

            break;

        case RUNNING:
            output_.status = MiSts::RUNNING;

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
            output_.status = MiSts::ERR;

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
AmkManagerBase::FsmState AmkManager<V1, SP>::Transition(const V1 val1,
                                                        const MiCmd cmd,
                                                        const int time_ms) {
    using enum FsmState;

    // If any of these states have amk_b_error set, move to ERROR_DETECTED state
    if ((amk_state_ == STARTUP_SYS_READY ||
         amk_state_ == STARTUP_TOGGLE_D_CON ||
         amk_state_ == STARTUP_ENFORCE_SETPOINTS_ZERO ||
         amk_state_ == STARTUP_COMMAND_ON || amk_state_ == READY ||
         amk_state_ == RUNNING) &&
        val1.AMK_bError()) {
        amk_state_start_time_ = time_ms;
        return ERROR_DETECTED;
    }

    FsmState new_state = amk_state_;
    int elapsed_time = time_ms - amk_state_start_time_;

    switch (amk_state_) {
        case MOTOR_OFF_WAITING_FOR_GOV:
            if (cmd == MiCmd::STARTUP) {
                new_state = STARTUP_SYS_READY;
            }

            break;

        case STARTUP_SYS_READY:
            if (val1.AMK_bSystemReady()) {
                new_state = STARTUP_TOGGLE_D_CON;
            }

            break;

        case STARTUP_TOGGLE_D_CON:
            if (val1.AMK_bDcOn() && val1.AMK_bQuitDcOn()) {
                new_state = STARTUP_ENFORCE_SETPOINTS_ZERO;
            }

            break;

        case STARTUP_ENFORCE_SETPOINTS_ZERO:
            if (elapsed_time >= 100) {
                new_state = STARTUP_COMMAND_ON;
            }

            break;

        case STARTUP_COMMAND_ON:
            if (val1.AMK_bInverterOn()) {
                new_state = READY;
            }

            break;

        case READY:
            if (val1.AMK_bQuitInverterOn()) {
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
            if (!val1.AMK_bInverterOn()) {
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
            if (val1.AMK_bSystemReady()) {
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