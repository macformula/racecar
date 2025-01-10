/// @author Blake Freer
/// @date 2024-02-24

#include "bindings.hpp"
#include "generated/can/pt_can_messages.hpp"
#include "generated/can/pt_msg_registry.hpp"
#include "generated/can/veh_can_messages.hpp"
#include "generated/can/veh_msg_registry.hpp"
#include "inc/app.hpp"
#include "inc/simulink.hpp"
#include "shared/comms/can/can_bus.hpp"
#include "shared/os/os.hpp"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/mapper.hpp"

/***************************************************************
    CAN
***************************************************************/

generated::can::VehMsgRegistry veh_can_registry{};
shared::can::CanBus veh_can_bus{
    bindings::veh_can_base,
    veh_can_registry,
};

generated::can::PtMsgRegistry pt_can_registry{};
shared::can::CanBus pt_can_bus{
    bindings::pt_can_base,
    pt_can_registry,
};

/***************************************************************
    Objects
***************************************************************/

ControlSystem control_system{};

Speaker driver_speaker{bindings::driver_speaker};
BrakeLight brake_light{bindings::brake_light};
StatusLight status_light{bindings::status_light};

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

auto brake_pedal_front_map = shared::util::LinearMap<double, uint16_t>{
    730.125,
    -0.29412,
};  // TODO: Check this, no function is given, copied from accel_pedal_1
AnalogInput brake_pedal_front{
    bindings::brake_pedal_front,
    &brake_pedal_front_map,
};

auto brake_pedal_rear_map = shared::util::LinearMap<double, uint16_t>{
    730.125,
    -0.29412,
};  // TODO: Check this, no function is given, copied from accel_pedal_1
AnalogInput brake_pedal_rear{
    bindings::brake_pedal_rear,
    &brake_pedal_rear_map,
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

AMKMotor motor_left{pt_can_bus, 1};
AMKMotor motor_right{pt_can_bus, 0};

Contactors contactors{veh_can_bus};

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
    input.DI_FrontBrakePressure = brake_pedal_front.Update();
    input.DI_RearBrakePressure = brake_pedal_rear.Update();
    input.DI_StartButton = start_button.Read();
    input.DI_AccelPedalPosition1 = accel_pedal_1.Update();
    input.DI_AccelPedalPosition2 = accel_pedal_2.Update();

    // Wheel Speed Sensors
    input.VD_LFWheelSpeed = NULL;
    input.VD_RFWheelSpeed = NULL;

    // Contactors
    auto contactor_states = contactors.ReadInput();
    input.BM_prechrgContactorSts = contactor_states.Pack_Precharge_Feedback;
    input.BM_HVposContactorSts = contactor_states.Pack_Negative_Feedback;
    input.BM_HVnegContactorSts = contactor_states.Pack_Positive_Feedback;
    input.BM_HvilFeedback = contactor_states.HvilFeedback;
    input.BM_LowThermValue = contactor_states.LowThermValue;
    input.BM_HighThermValue = contactor_states.HighThermValue;
    input.BM_AvgThermValue = contactor_states.AvgThermValue;
    input.BM_PackSOC = contactor_states.PackSOC;

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

    brake_light.Update(output.DI_BrakeLightEn);

    // @todo This Update() is not implemented
    // status_light.Update(output.DI_p_PWMstatusLightCycle,
    //                     output.DI_PWMstatusLightFreq);

    // TODO the following 2 outputs
    auto foo = output.GOV_Status;
    auto bar = output.MI_InverterEn;

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
        // .prechargeContactorCMD = output.BM_PrechargeContactorCmd,
        // .HVposContactorCMD = output.BM_HVposContactorCmd,
        // .HVnegContactorCMD = output.BM_HVnegContactorCmd, // TODO uncomment
    });
}

/**
 * @brief Update the status light
 * @note Replaces setPWMOutputs()
 *
 */
void UpdateStatusLight() {
    status_light.Toggle();
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