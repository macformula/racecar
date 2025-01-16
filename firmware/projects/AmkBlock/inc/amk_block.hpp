#pragma once

#include "../generated/can/pt_can_messages.hpp"
#include <concepts>

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

// MotorInput struct used in AmkInput, this is defined to encapsulate identical values for left and right
struct MotorInput {
    double speed_request;
    double torque_limit_negative;
    double torque_limit_positive;
};

// AmkInput struct, the input that is fed into the state machine
struct AmkInput {
    MiCmd cmd;
    generated::can::AMK0_ActualValues1 amk_actual_values_1_left;
    generated::can::AMK0_ActualValues2 amk_actual_values_2_left;
    generated::can::AMK1_ActualValues1 amk_actual_values_1_right;
    generated::can::AMK1_ActualValues2 amk_actual_values_2_right;
    MotorInput left_motor_input;
    MotorInput right_motor_input;
};

// AmkOutput struct, the output that is produced from the state machine
struct AmkOutput {
    MiStatus status;
    generated::can::AMK0_SetPoints1 amk_setpoints_1_left;
    generated::can::AMK1_SetPoints1 amk_setpoints_1_right;
    bool inverter;
};

// AmkActualValues1 concept, combines AMK0_ActualValues1 and AMK1_ActualValues1 into one common type for easier use
template <typename MsgType>
concept AmkActualValues1 = requires(MsgType msg) {
    { msg.amk_b_system_ready }; // TODO: Add the std::same_as<bool> check, currently it is erroring out
    { msg.amk_b_error };
    { msg.amk_b_warn };
    { msg.amk_b_quit_dc_on };
    { msg.amk_b_dc_on };
    { msg.amk_b_quit_inverter_on };
    { msg.amk_b_inverter_on };
    { msg.amk_b_derating };
    { msg.amk__actual_velocity };
    { msg.amk__torque_current };
    { msg.amk__magnetizing_current };
};

// AmkActualValues2 concept, combines AMK0_ActualValues2 and AMK1_ActualValues2 into one common type for easier use
template<typename MsgType>
concept AmkActualValues2 = requires(MsgType msg) {
    { msg.amk__temp_motor };
    { msg.amk__temp_inverter };
    { msg.amk__error_info };
    { msg.amk__temp_igbt };
};

// SetPoint concept, combines AMK0_SetPoints1 and AMK1_SetPoints1 into one common type for easier use
template<typename MsgType>
concept SetPoint = requires(MsgType msg) {
    { msg.amk_b_inverter_on };
    { msg.amk_b_dc_on };
    { msg.amk_b_enable };
    { msg.amk_b_error_reset };
    { msg.amk__target_velocity };
    { msg.amk__torque_limit_positiv };
    { msg.amk__torque_limit_negativ };
};

// UpdateMotorOutput struct, used to create a return type for UpdateMotor such that it does not matter if set_point is for left or right
template<SetPoint SP>
struct UpdateMotorOutput {
    SP set_point;
    MiStatus status;
    bool inverter;
};

// AmkStates enum class which defines all states in the amk state machine
enum class AmkStates {
    MOTOR_OFF_WAITING_FOR_GOV,
    STARTUP,
    READY,
    RUNNING,
    SHUTDOWN,
    ERROR_DETECTED,
    ERROR_RESET
};

class AmkBlock {
public:
    AmkBlock(AmkStates initial_state) : amk_state(initial_state) {}

    AmkOutput update(const AmkInput& input, int time_ms) {
        UpdateMotorOutput<generated::can::AMK0_SetPoints1> left_update_motor_output = UpdateMotor<generated::can::AMK0_ActualValues1, generated::can::AMK0_ActualValues2, generated::can::AMK0_SetPoints1>(input.amk_actual_values_1_left, input.amk_actual_values_2_left, input.left_motor_input, input.cmd);
        UpdateMotorOutput<generated::can::AMK1_SetPoints1> right_update_motor_output = UpdateMotor<generated::can::AMK1_ActualValues1, generated::can::AMK1_ActualValues2, generated::can::AMK1_SetPoints1>(input.amk_actual_values_1_right, input.amk_actual_values_2_right, input.right_motor_input, input.cmd);

        AmkOutput output {
            .status = process_output_status(left_update_motor_output.status, right_update_motor_output.status),
            .amk_setpoints_1_left = left_update_motor_output.set_point,
            .amk_setpoints_1_right = right_update_motor_output.set_point,
            .inverter = left_update_motor_output.inverter && right_update_motor_output.inverter
        };

        return output;
    }

private:
    AmkStates amk_state;
    MiStatus previous_state_status = MiStatus::OFF;

    template<AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
    UpdateMotorOutput<SP> UpdateMotor(const V1 val1, const V2 val2, const MotorInput motor_input, const MiCmd cmd) {
        UpdateMotorOutput<SP> update_motor_output;
        SP set_point;

        switch (amk_state) {
            case AmkStates::MOTOR_OFF_WAITING_FOR_GOV:
                update_motor_output.status = MiStatus::OFF;
                update_motor_output.inverter = 0;

                set_point.amk_b_inverter_on = 0;
                set_point.amk_b_dc_on = 0;
                set_point.amk_b_enable = 0;
                set_point.amk_b_error_reset = 0;
                set_point.amk__target_velocity = 0;
                set_point.amk__torque_limit_positiv = 0;
                set_point.amk__torque_limit_negativ = 0;

                if (cmd == MiCmd::STARTUP) {
                    amk_state = AmkStates::STARTUP;
                }

                break;

            case AmkStates::STARTUP:
                // Create new function for nested state machine block
                
                if (val1.amk_b_error == 1) { // Check if this is correct logic since arrow has no logic
                    amk_state = AmkStates::ERROR_DETECTED;
                }
                else if (val1.amk_b_inverter_on == 1) {
                    amk_state = AmkStates::READY;
                }

                break;

            case AmkStates::READY:
                update_motor_output.status = MiStatus::READY;
                update_motor_output.inverter = 0;
                
                if (val1.amk_b_error == 1) {
                    amk_state = AmkStates::ERROR_DETECTED;
                }
                else if (val1.amk_b_quit_inverter_on == 1) {
                    amk_state = AmkStates::RUNNING;
                }

                break;

            case AmkStates::RUNNING:
                update_motor_output.status = MiStatus::RUNNING;

                set_point.amk__target_velocity = motor_input.speed_request;
                set_point.amk__torque_limit_positiv = motor_input.torque_limit_positive;
                set_point.amk__torque_limit_negativ = motor_input.torque_limit_negative;

                if (val1.amk_b_error == 1) {
                    amk_state = AmkStates::ERROR_DETECTED;
                }
                else if (cmd == MiCmd::SHUTDOWN) {
                    amk_state == AmkStates::SHUTDOWN;
                }

                break;

            case AmkStates::SHUTDOWN:
                update_motor_output.inverter = 0;

                set_point.amk__target_velocity = 0;
                set_point.amk__torque_limit_positiv = 0;
                set_point.amk__torque_limit_negativ = 0;
                // Rest of setpoint values do not get set
                // I'm thinking we have to have private var hold previous output, set SP set_point = previous_set_point at start of UpdateMotor
                // This way on every call of UpdateMotor, if an output value is not set, it's cause the prev is retained

                // Set some time condition with after(500, msec)

                break;

            case AmkStates::ERROR_DETECTED:
                update_motor_output.status = MiStatus::ERROR;

                if (cmd == MiCmd::ERR_RESET) {
                    amk_state = AmkStates::ERROR_RESET;
                }

                break;

            case AmkStates::ERROR_RESET:
                // Create new function for nested state machine block

                if (val1.amk_b_system_ready == 1) {
                    amk_state = AmkStates::MOTOR_OFF_WAITING_FOR_GOV;
                }

                break;
        }

        return update_motor_output;
    }

    MiStatus process_output_status(MiStatus left_motor_status, MiStatus right_motor_status) {
        MiStatus output_status;
        if (left_motor_status == MiStatus::ERROR || right_motor_status == MiStatus::ERROR) {
            output_status = MiStatus::ERROR;
        }
        else if (left_motor_status == right_motor_status) {
            output_status = left_motor_status;
        }
        else {
            output_status = previous_state_status;
        }

        previous_state_status = output_status;
        return output_status;
    }
};