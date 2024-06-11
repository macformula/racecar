/// @author Blake Freer
/// @date 2024-02-24

#include "app.h"
#include "bindings.h"
#include "controller_autogen.h"
#include "shared/os/os.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/linear_map.h"
#include "shared/util/mappers/mapper.h"
#include "simulink.h"

/***************************************************************
    Objects
***************************************************************/

Speaker driver_speaker{bindings::driver_speaker};
BrakeLight brake_light{bindings::brake_light};
StatusLight status_light{bindings::status_light};
ControlSystem control_system{};

// See fc_docs/pedal_function and
// vehicle_control_system/firmware_io/simulink_input.csv
auto accel_pedal_1_map = shared::util::LinearMap<double, uint16_t>{
    730.125,
    -0.29412,
};  // Asumming 3.3V = 4096
AnalogInput accel_pedal_1{
    bindings::accel_pedal_1,
    &accel_pedal_1_map,
};

auto accel_pedal_2_map = shared::util::LinearMap<double, uint16_t>{
    730.125,
    -0.4706,
};  // Asumming 3.3V = 4096
AnalogInput accel_pedal_2{
    bindings::accel_pedal_2,
    &accel_pedal_2_map,
};

auto brake_pedal_map = shared::util::LinearMap<double, uint16_t>{
    730.125,
    -0.29412,
};  // TODO: Check this, no function is given, copied from accel_pedal_1
AnalogInput front_brake_pedal{
    bindings::front_brake_pedal,
    &brake_pedal_map,
};

auto steering_wheel_map = shared::util::LinearMap<double, uint16_t>{
    1.0 / 4096.0,
    0,
};  // Assuming 3.3V = 4096
AnalogInput steering_wheel{
    bindings::steering_wheel,
    &steering_wheel_map,
};

Button start_button{bindings::start_button};

AMKMotor motor_left{bindings::motor_left_can};
AMKMotor motor_right{bindings::motor_right_can};

Contactors contactors{bindings::contactor_can};

/***************************************************************
    Task Functions
***************************************************************/

extern "C" {
void Task_5ms(void* argument);
void Task_500ms(void* argument);
}

/***************************************************************
    RTOS
***************************************************************/
namespace os {
extern void TickUntil(uint32_t ticks);
extern uint32_t GetTickCount();
extern void InitializeKernel();
extern void StartKernel();
}  // namespace os

void ReadContactorFeedback(SimulinkInput& input) {}

/**
 * @brief Read the inputs from all devices to send to the control system.
 * @note Replaces readFromAdc()
 *
 * @return SimulinkInput
 */
SimulinkInput ReadCtrlSystemInput() {
    SimulinkInput input;

    // Driver Input
    input.DI_SteeringAngle = steering_wheel.Update();
    input.DI_AccelPedalPosition1 = accel_pedal_1.Update();
    input.DI_AccelPedalPosition2 = accel_pedal_2.Update();
    input.DI_FrontBrakePressure = front_brake_pedal.Update();
    input.DI_StartButton = start_button.Read();

    // Contactors
    auto contactor_states = contactors.ReadInput();
    input.BM_prechrgContactorSts = contactor_states.Pack_Precharge_Feedback;
    input.BM_HVposContactorSts = contactor_states.Pack_Negative_Feedback;
    input.BM_HVnegContactorSts = contactor_states.Pack_Positive_Feedback;

    // Right Motor Input
    auto amk_right_in = motor_right.UpdateInputs();
    input.AMK0_bReserve = amk_right_in.bReserve;
    input.AMK0_bSystemReady = amk_right_in.bSystemReady;
    input.AMK0_bError = amk_right_in.bError;
    input.AMK0_bWarn = amk_right_in.bWarn;
    input.AMK0_bQuitDcOn = amk_right_in.bQuitDcOn;
    input.AMK0_bDcOn = amk_right_in.bDcOn;
    input.AMK0_bQuitInverterOn = amk_right_in.bQuitInverterOn;
    input.AMK0_bInverterOn = amk_right_in.bInverterOn;
    input.AMK0_bDerating = amk_right_in.bDerating;
    input.AMK0_ActualVelocity = amk_right_in.ActualVelocity;
    input.AMK0_TorqueCurrent = amk_right_in.TorqueCurrent;
    input.AMK0_MagnetizingCurrent = amk_right_in.MagnetizingCurrent;
    input.AMK0_TempMotor = amk_right_in.TempMotor;
    input.AMK0_TempInverter = amk_right_in.TempInverter;
    input.AMK0_ErrorInfo = amk_right_in.ErrorInfo;
    input.AMK0_TempIGBT = amk_right_in.TempIGBT;

    // Left Motor Input
    auto amk_left_in = motor_left.UpdateInputs();
    input.AMK1_bReserve = amk_left_in.bReserve;
    input.AMK1_bSystemReady = amk_left_in.bSystemReady;
    input.AMK1_bError = amk_left_in.bError;
    input.AMK1_bWarn = amk_left_in.bWarn;
    input.AMK1_bQuitDcOn = amk_left_in.bQuitDcOn;
    input.AMK1_bDcOn = amk_left_in.bDcOn;
    input.AMK1_bQuitInverterOn = amk_left_in.bQuitInverterOn;
    input.AMK1_bInverterOn = amk_left_in.bInverterOn;
    input.AMK1_bDerating = amk_left_in.bDerating;
    input.AMK1_ActualVelocity = amk_left_in.ActualVelocity;
    input.AMK1_TorqueCurrent = amk_left_in.TorqueCurrent;
    input.AMK1_MagnetizingCurrent = amk_left_in.MagnetizingCurrent;
    input.AMK1_TempMotor = amk_left_in.TempMotor;
    input.AMK1_TempInverter = amk_left_in.TempInverter;
    input.AMK1_ErrorInfo = amk_left_in.ErrorInfo;
    input.AMK1_TempIGBT = amk_left_in.TempIGBT;

    return input;

    /* These inputs are listed in simulink_input.csv but are not present in the
    simulink code, so we cannot possibly set them right now.

    input.VD_FrtWheelSpd = NULL;
    input.BM_HvilFeedback = NULL;
    input.BM_PackSOC = NULL;
    input.BM_LowThermValue = NULL;
    input.BM_HighThermValue = NULL;
    input.BM_AvgThermValue = NULL;
    input.VD_AccelerationX = NULL;
    input.VD_AccelerationY = NULL;
    input.VD_AccelerationZ = NULL;
    input.VD_AngularRateX = NULL;
    input.VD_AngularRateY = NULL;
    input.VD_AngularRateZ = NULL;
    input.VD_ImuValid = NULL;
    */
}

/**
 * @brief Update output devices based on the results of the control system.
 * @note Replaces getControlSystemOutputs() setDigitalOutputs(),
 * transmitToAMKMotors(), and transmitToBMS()
 *
 * @param output
 */
void SetCtrlSystemOutput(const SimulinkOutput& output) {
    driver_speaker.Update(output.DI_DriverSpeaker);

    // why is DI_b_brakeLightEn from  simulink output a float? Should be bool
    // since it is "enable?"
    brake_light.Update(output.DI_BrakeLightEn);

    // @todo This Update() is not implemented
    // status_light.Update(output.DI_p_PWMstatusLightCycle,
    //                     output.DI_PWMstatusLightFreq);

    motor_right.Transmit(AMKOutput{
        .bInverterOn_tx = output.AMK0_bInverterOn_tx,
        .bDcOn_tx = output.AMK0_bDcOn_tx,
        .bEnable = output.AMK0_bEnable,
        .bErrorReset = output.AMK0_bErrorReset,
        .TargetVelocity = output.AMK0_TargetVelocity,
        .TorqueLimitPositiv = output.AMK0_TorqueLimitPositiv,
        .TorqueLimitNegativ = output.AMK0_TorqueLimitNegativ,
    });
    motor_left.Transmit(AMKOutput{
        .bInverterOn_tx = output.AMK1_bInverterOn_tx,
        .bDcOn_tx = output.AMK1_bDcOn_tx,
        .bEnable = output.AMK1_bEnable,
        .bErrorReset = output.AMK1_bErrorReset,
        .TargetVelocity = output.AMK1_TargetVelocity,
        .TorqueLimitPositiv = output.AMK1_TorqueLimitPositiv,
        .TorqueLimitNegativ = output.AMK1_TorqueLimitNegativ,
    });

    contactors.Transmit(ContactorOutput{
        .prechargeContactorCMD = output.BM_PrechargeContactorCmd,
        .HVposContactorCMD = output.BM_HVposContactorCmd,
        .HVnegContactorCMD = output.BM_HVnegContactorCmd,
    });
}

/**
 * @brief Update the status light
 * @note Replaces setPWMOutputs()
 *
 */
void UpdateStatusLight() {
    // status_light.DoSomething();

    /* (SAM) what are we actually doing? I thought the status light had 3
    states: off, on, and blinking at a (non-adjustable) frequency.

    However, the vehicle_control_system/firmware_io/simulink_output.csv
    document contains both a pwm for brightness and a frequency of flashing.

    I am not sure how both of these can be accomplished together (either is
    possible using a single timer) and why the brightness modulation is even
    needed.

    Also, 500 ms seems like a long period for updating a status light.
    Why can't it be contained in the 5 ms task?
    */
}

/***************************************************************
    RTOS Tasks
***************************************************************/

void Task_5ms(void* arg) {
    SimulinkInput simulink_input;
    SimulinkOutput simulink_output;

    uint32_t task_delay_ms = os::GetTickCount();

    while (true) {
        simulink_input = ReadCtrlSystemInput();
        simulink_output = control_system.Update(&simulink_input);
        SetCtrlSystemOutput(simulink_output);

        // Repeat after another 5ms
        task_delay_ms += 5;
        os::TickUntil(task_delay_ms);
    }
}

void Task_500ms(void* arg) {
    uint32_t task_delay_ms = os::GetTickCount();

    while (true) {
        UpdateStatusLight();

        // Repeat after another 500ms
        task_delay_ms += 500;
        os::TickUntil(task_delay_ms);
    }
}

int main(void) {
    bindings::Initialize();
    control_system.Initialize();

    os::InitializeKernel();

    os::StartKernel();

    while (true) continue;  // logic is handled by OS tasks

    return 0;
}