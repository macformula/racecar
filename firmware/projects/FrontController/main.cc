/// @author Blake Freer
/// @date 2024-02-24

#include "bindings.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/app.hpp"
#include "inc/simulink.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/util/mappers/linear_map.hpp"

/***************************************************************
    CAN
***************************************************************/
using namespace generated::can;

VehBus veh_can_bus{bindings::veh_can_base};
PtBus pt_can_bus{bindings::pt_can_base};

/***************************************************************
    Objects
***************************************************************/

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

/**
 * @brief Read the inputs from all devices to send to the control system.
 * @note Replaces readFromAdc()
 *
 * @return SimulinkInput
 */
SimulinkInput ReadCtrlSystemInput() {
    SimulinkInput input{};  // zero initialize struct

    // Driver Input
    input.DI_SteeringAngle = steering_wheel.Update();
    input.DI_FrontBrakePressure = brake_pedal_front.Update();
    input.DI_RearBrakePressure = brake_pedal_rear.Update();
    input.DI_StartButton = bindings::start_button.Read();
    input.DI_AccelPedalPosition1 = accel_pedal_1.Update();
    input.DI_AccelPedalPosition2 = accel_pedal_2.Update();

    // Wheel Speed Sensors
    input.VD_LFWheelSpeed = NULL;
    input.VD_RFWheelSpeed = NULL;

    // Contactors
    auto contactor_states = veh_can_bus.GetRxContactor_Feedback();
    if (contactor_states) {
        input.BM_prechrgContactorSts =
            contactor_states->Pack_Precharge_Feedback();
        // these were swapped in the 2023-24 code. i assume this was an error
        input.BM_HVposContactorSts = contactor_states->Pack_Positive_Feedback();
        input.BM_HVnegContactorSts = contactor_states->Pack_Negative_Feedback();
    }  // else values are already zero initialized

    input.BM_HvilFeedback = contactor_states.HvilFeedback;
    input.BM_LowThermValue = contactor_states.LowThermValue;
    input.BM_HighThermValue = contactor_states.HighThermValue;
    input.BM_AvgThermValue = contactor_states.AvgThermValue;
    input.BM_PackSOC = contactor_states.PackSOC;

    // Right Motor Input
    auto amk0_actual1 = pt_can_bus.GetRxAMK0_ActualValues1();
    if (amk0_actual1) {
        input.AMK0_bSystemReady = amk0_actual1->AMK_bSystemReady();
        input.AMK0_bError = amk0_actual1->AMK_bError();
        input.AMK0_bWarn = amk0_actual1->AMK_bWarn();
        input.AMK0_bQuitDcOn = amk0_actual1->AMK_bQuitDcOn();
        input.AMK0_bDcOn = amk0_actual1->AMK_bDcOn();
        input.AMK0_bQuitInverterOn = amk0_actual1->AMK_bQuitInverterOn();
        input.AMK0_bInverterOn = amk0_actual1->AMK_bInverterOn();
        input.AMK0_bDerating = amk0_actual1->AMK_bDerating();
        input.AMK0_ActualVelocity = amk0_actual1->AMK_ActualVelocity();
        input.AMK0_TorqueCurrent = amk0_actual1->AMK_TorqueCurrent();
        input.AMK0_MagnetizingCurrent = amk0_actual1->AMK_MagnetizingCurrent();
    }
    auto amk0_actual2 = pt_can_bus.GetRxAMK0_ActualValues2();
    if (amk0_actual2) {
        input.AMK0_TempMotor = amk0_actual2->AMK_TempMotor();
        input.AMK0_TempInverter = amk0_actual2->AMK_TempInverter();
        input.AMK0_ErrorInfo = amk0_actual2->AMK_ErrorInfo();
        input.AMK0_TempIGBT = amk0_actual2->AMK_TempIGBT();
    }

    // Left Motor Input
    auto amk1_actual1 = pt_can_bus.GetRxAMK1_ActualValues1();
    if (amk1_actual1) {
        input.AMK1_bSystemReady = amk1_actual1->AMK_bSystemReady();
        input.AMK1_bError = amk1_actual1->AMK_bError();
        input.AMK1_bWarn = amk1_actual1->AMK_bWarn();
        input.AMK1_bQuitDcOn = amk1_actual1->AMK_bQuitDcOn();
        input.AMK1_bDcOn = amk1_actual1->AMK_bDcOn();
        input.AMK1_bQuitInverterOn = amk1_actual1->AMK_bQuitInverterOn();
        input.AMK1_bInverterOn = amk1_actual1->AMK_bInverterOn();
        input.AMK1_bDerating = amk1_actual1->AMK_bDerating();
        input.AMK1_ActualVelocity = amk1_actual1->AMK_ActualVelocity();
        input.AMK1_TorqueCurrent = amk1_actual1->AMK_TorqueCurrent();
        input.AMK1_MagnetizingCurrent = amk1_actual1->AMK_MagnetizingCurrent();
    }
    auto amk1_actual2 = pt_can_bus.GetRxAMK1_ActualValues2();
    if (amk1_actual2) {
        input.AMK1_TempMotor = amk1_actual2->AMK_TempMotor();
        input.AMK1_TempInverter = amk1_actual2->AMK_TempInverter();
        input.AMK1_ErrorInfo = amk1_actual2->AMK_ErrorInfo();
        input.AMK1_TempIGBT = amk1_actual2->AMK_TempIGBT();
    }

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
    bindings::driver_speaker.Set(output.DI_DriverSpeaker);

    bindings::brake_light.Set(output.DI_BrakeLightEn);

    // @todo This Update() is not implemented
    // status_light.Update(output.DI_p_PWMstatusLightCycle,
    //                     output.DI_PWMstatusLightFreq);

    // TODO the following 2 outputs
    auto foo = output.GOV_Status;
    auto bar = output.MI_InverterEn;

    // @todo the bool types should be set in simulink
    // AMK0 = right motor
    veh_can_bus.Send(TxAMK0_SetPoints1{
        .amk_b_inverter_on = static_cast<bool>(output.AMK0_bInverterOn_tx),
        .amk_b_dc_on = static_cast<bool>(output.AMK0_bDcOn_tx),
        .amk_b_enable = static_cast<bool>(output.AMK0_bEnable),
        .amk_b_error_reset = static_cast<bool>(output.AMK0_bErrorReset),
        .amk__target_velocity =
            static_cast<int16_t>(output.AMK0_TargetVelocity),
        .amk__torque_limit_positiv =
            static_cast<int16_t>(output.AMK0_TorqueLimitPositiv),
        .amk__torque_limit_negativ =
            static_cast<int16_t>(output.AMK0_TorqueLimitNegativ),
    });
    // AMK1 = left motor
    veh_can_bus.Send(TxAMK1_SetPoints1{
        .amk_b_inverter_on = static_cast<bool>(output.AMK1_bInverterOn_tx),
        .amk_b_dc_on = static_cast<bool>(output.AMK1_bDcOn_tx),
        .amk_b_enable = static_cast<bool>(output.AMK1_bEnable),
        .amk_b_error_reset = static_cast<bool>(output.AMK1_bErrorReset),
        .amk__target_velocity =
            static_cast<int16_t>(output.AMK1_TargetVelocity),
        .amk__torque_limit_positiv =
            static_cast<int16_t>(output.AMK1_TorqueLimitPositiv),
        .amk__torque_limit_negativ =
            static_cast<int16_t>(output.AMK1_TorqueLimitNegativ),
    });

    veh_can_bus.Send(TxContactorStates{
        .pack_positive = static_cast<uint8_t>(output.BM_HVposContactorCmd),
        .pack_precharge = static_cast<uint8_t>(output.BM_PrechargeContactorCmd),
        .pack_negative = static_cast<uint8_t>(output.BM_HVnegContactorCmd),
    });
}

/**
 * @brief Update the status light
 * @note Replaces setPWMOutputs()
 *
 */
void UpdateStatusLight() {
    static bool toggle = true;
    bindings::status_light.Set(toggle);
    toggle = !toggle;
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