#pragma once

#include "../generated/can/pt_can_messages.hpp"
#include <concepts>

// MiStatus class used in AmkInput
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

// MiCmd class used in AmkInput
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

// Make enum of all possible states in the simulink FSM and then add cases logic and transitions

class AmkBlock {
public:
    AmkBlock();
    AmkOutput update(const AmkInput& input, int time_ms) {
        UpdateMotorOutput<generated::can::AMK0_SetPoints1> left_update_motor_output = UpdateMotor<generated::can::AMK0_ActualValues1, generated::can::AMK0_ActualValues2, generated::can::AMK0_SetPoints1>(input.amk_actual_values_1_left, input.amk_actual_values_2_left, input.left_motor_input, input.cmd);
        UpdateMotorOutput<generated::can::AMK1_SetPoints1> right_update_motor_output = UpdateMotor<generated::can::AMK1_ActualValues1, generated::can::AMK1_ActualValues2, generated::can::AMK1_SetPoints1>(input.amk_actual_values_1_right, input.amk_actual_values_2_right, input.right_motor_input, input.cmd);

        AmkOutput output;

        return output;
    }

private:
    template<AmkActualValues1 V1, AmkActualValues2 V2, SetPoint SP>
    UpdateMotorOutput<SP> UpdateMotor(const V1 val1, const V2 val2, const MotorInput motor_input, const MiCmd cmd) {
        UpdateMotorOutput<SP> update_motor_output;

        return update_motor_output;
    }
};