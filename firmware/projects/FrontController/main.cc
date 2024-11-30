/// @author Blake Freer
/// @date 2024-02-24

#include "bindings.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
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

// Asumming 3.3V = 4096
double ReadApps1() {
    const auto map =
        shared::util::LinearMap<double, uint16_t>{730.125, -0.29412};
    return map.Evaluate(bindings::accel_pedal_1.Read());
}

double ReadApps2() {
    const auto map =
        shared::util::LinearMap<double, uint16_t>{730.125, -0.4706};
    return map.Evaluate(bindings::accel_pedal_2.Read());
}

// TODO: Check BPPS maps. no function is given, copied from accel_pedal_1
double ReadBrakePedalFront() {
    const auto map =
        shared::util::LinearMap<double, uint16_t>{730.125, -0.29412};
    return map.Evaluate(bindings::brake_pedal_front.Read());
}

double ReadBrakePedalRear() {
    const auto map =
        shared::util::LinearMap<double, uint16_t>{730.125, -0.29412};
    return map.Evaluate(bindings::brake_pedal_rear.Read());
}

double ReadSteeringWheel() {
    const auto map = shared::util::LinearMap<double, uint16_t>{1.0 / 4096.0, 0};
    return map.Evaluate(bindings::steering_wheel.Read());
}

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

/***************************************************************
    Functions
***************************************************************/

enum class StatusLightMode {
    OFF,
    FLASHING,
    SOLID,
};
volatile StatusLightMode status_light_mode = StatusLightMode::OFF;

void UpdateStatusLight() {
    static bool next_toggle = true;

    switch (status_light_mode) {
        case StatusLightMode::OFF:
            bindings::status_light.Set(false);
            next_toggle = true;
            break;

        case StatusLightMode::SOLID:
            bindings::status_light.Set(true);
            next_toggle = false;
            break;

        case StatusLightMode::FLASHING:
            bindings::status_light.Set(next_toggle);
            next_toggle = !next_toggle;
            break;
    }
}

struct DriverInput {
    double steering_angle;
    double front_brake_pressure;
    double rear_brake_pressure;
    bool start_button_pressed;
    double accel_pedal_position1;
    double accel_pedal_position2;
    bool hvil_status;
};

DriverInput ReadDriverInput() {
    return DriverInput{
        .steering_angle = ReadSteeringWheel(),
        .front_brake_pressure = ReadBrakePedalFront(),
        .rear_brake_pressure = ReadBrakePedalRear(),
        .start_button_pressed = bindings::start_button.Read(),
        .accel_pedal_position1 = ReadApps1(),
        .accel_pedal_position2 = ReadApps2(),
        .hvil_status = false,  // is this going away in EV6?
    };
}
/// @brief Read the inputs from all devices to send to the control system.
/// @return SimulinkInput
SimulinkInput ReadCtrlSystemInput() {
    SimulinkInput input{};  // zero initialize struct

    auto driver_input = ReadDriverInput();
    veh_can_bus.Send(TxFC_msg{
        .fc_apps1 = static_cast<float>(driver_input.accel_pedal_position1),
        .fc_apps2 = static_cast<float>(driver_input.accel_pedal_position2),
        .fc_bpps = static_cast<float>(driver_input.front_brake_pressure),
        .fc_steering_angle = static_cast<float>(driver_input.steering_angle),
        .fc_hvil_sts = driver_input.hvil_status,
        .fc_ready_to_drive_btn_n = driver_input.start_button_pressed,
    });

    // Driver Input
    input.DI_SteeringAngle = driver_input.steering_angle;
    input.DI_FrontBrakePressure = driver_input.front_brake_pressure;
    input.DI_RearBrakePressure = driver_input.rear_brake_pressure;
    input.DI_StartButton = driver_input.start_button_pressed;
    input.DI_AccelPedalPosition1 = driver_input.accel_pedal_position1;
    input.DI_AccelPedalPosition2 = driver_input.accel_pedal_position2;

    // Wheel Speed Sensors - do these exist?
    input.VD_LFWheelSpeed = 0;
    input.VD_RFWheelSpeed = 0;
    input.BM_HvilFeedback = driver_input.hvil_status;

    // Contactors
    auto contactor_states = veh_can_bus.GetRxContactor_Feedback();
    if (contactor_states) {
        input.BM_prechrgContactorSts =
            contactor_states->Pack_Precharge_Feedback();
        // these were swapped in the EV5 code. i assume this was an error
        input.BM_HVposContactorSts = contactor_states->Pack_Positive_Feedback();
        input.BM_HVnegContactorSts = contactor_states->Pack_Negative_Feedback();
    }  // else values are already zero initialized

    auto bm_temperatures = veh_can_bus.GetRxBmsBroadcast();
    if (bm_temperatures) {
        input.BM_LowThermValue = bm_temperatures->LowThermValue();
        input.BM_HighThermValue = bm_temperatures->HighThermValue();
        input.BM_AvgThermValue = bm_temperatures->AvgThermValue();
    }

    auto bms_states = veh_can_bus.GetRxPack_SOC();
    if (bms_states) {
        input.BM_PackSOC = bms_states->Pack_SOC();
    }

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

/// @brief Update output devices based on the results of the control system.
void SetCtrlSystemOutput(const SimulinkOutput& output) {
    bindings::driver_speaker.Set(output.DI_DriverSpeaker);
    bindings::brake_light.Set(output.DI_BrakeLightEn);

    // @todo This Update() is not implemented
    // there is no login in the simulink model to update the status light
    status_light_mode = StatusLightMode::SOLID;

    veh_can_bus.Send(TxVC_Status{
        .vc_gov_status = output.GOV_Status,
        // remaning statuses are not part of simulink output
        .vc_di_status = 0,
        .vc_mi_status = 0,
        .vc_bm_status = 0,
    });

    veh_can_bus.Send(TxInverterCommand{
        .enable_inverter = static_cast<bool>(output.MI_InverterEn)});

    // @todo Simulink output should use the bool type
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

/***************************************************************
    RTOS Tasks
***************************************************************/

void Task_5ms(void* arg) {
    (void)arg;  // unused

    uint32_t next_execution_time = os::GetTickCount();

    while (true) {
        SimulinkInput input = ReadCtrlSystemInput();
        SimulinkOutput output = control_system.Update(&input);
        SetCtrlSystemOutput(output);

        next_execution_time += 5;
        os::TickUntil(next_execution_time);
    }
}

void Task_500ms(void* arg) {
    (void)arg;  // unused

    uint32_t next_execution_time = os::GetTickCount();

    while (true) {
        UpdateStatusLight();

        next_execution_time += 500;
        os::TickUntil(next_execution_time);
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