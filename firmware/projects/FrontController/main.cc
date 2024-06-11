/// @author Blake Freer
/// @date 2024-02-24

#include <sys/_stdint.h>

#include "app.h"
#include "bindings.h"
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
AnalogInput brake_pedal{
    bindings::brake_pedal,
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

Button driver_button{bindings::driver_button};

AMKMotor motor_left{};
AMKMotor motor_right{};

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

    input.DI_V_SteeringAngle = steering_wheel.Update();
    input.DI_V_AccelPedalPos1 = accel_pedal_1.Update();

    input.DI_V_AccelPedalPos2 = accel_pedal_2.Update();
    input.DI_V_BrakePedalPos = brake_pedal.Update();

    input.DI_b_DriverButton = driver_button.Read();

    /* Sam: Fill in the following template. You can create an app-level

    // Contactor class&object if needed
    auto contactor_states = CAN.ReadContactor();
    input.BM_b_prechrgContactorSts = contactor_states.Pack_Precharge_Feedback;
    input.BM_b_HVposContactorSts = contactor_states.Pack_Negative_Feedback;
    input.BM_b_HVnegContactorSts = contactor_states.Pack_Positive_Feedback;
    */

    // Right Motor Input
    auto amk_r = motor_right.UpdateInputs();
    input.AMK_bReserve_R = amk_r.bReserve;
    input.AMK_bSystemReady_R = amk_r.bSystemReady;
    input.AMK_bError_R = amk_r.bError;
    input.AMK_bWarn_R = amk_r.bWarn;
    input.AMK_bQuitDcOn_R = amk_r.bQuitDcOn;
    input.AMK_bDcOn_R = amk_r.bDcOn;
    input.AMK_bQuitInverterOn_R = amk_r.bQuitInverterOn;
    input.AMK_bInverterOn_R = amk_r.bInverterOn;
    input.AMK_bDerating_R = amk_r.bDerating;
    input.AMK_ActualVelocity_R = amk_r.ActualVelocity;
    input.AMK_TorqueCurrent_R = amk_r.TorqueCurrent;
    input.AMK_MagnetizingCurrent_R = amk_r.MagnetizingCurrent;
    input.AMK_TempMotor_R = amk_r.TempMotor;
    input.AMK_TempInverter_R = amk_r.TempInverter;
    input.AMK_ErrorInfo_R = amk_r.ErrorInfo;
    input.AMK_TempIGBT_R = amk_r.TempIGBT;

    // Left Motor Input
    auto amk_l = motor_left.UpdateInputs();
    input.AMK_bReserve_L = amk_l.bReserve;
    input.AMK_bSystemReady_L = amk_l.bSystemReady;
    input.AMK_bError_L = amk_l.bError;
    input.AMK_bWarn_L = amk_l.bWarn;
    input.AMK_bQuitDcOn_L = amk_l.bQuitDcOn;
    input.AMK_bDcOn_L = amk_l.bDcOn;
    input.AMK_bQuitInverterOn_L = amk_l.bQuitInverterOn;
    input.AMK_bInverterOn_L = amk_l.bInverterOn;
    input.AMK_bDerating_L = amk_l.bDerating;
    input.AMK_ActualVelocity_L = amk_l.ActualVelocity;
    input.AMK_TorqueCurrent_L = amk_l.TorqueCurrent;
    input.AMK_MagnetizingCurrent_L = amk_l.MagnetizingCurrent;
    input.AMK_TempMotor_L = amk_l.TempMotor;
    input.AMK_TempInverter_L = amk_l.TempInverter;
    input.AMK_ErrorInfo_L = amk_l.ErrorInfo;
    input.AMK_TempIGBT_L = amk_l.TempIGBT;

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

    return input;
}

/**
 * @brief Update output devices based on the results of the control system.
 * @note Replaces getControlSystemOutputs() and setDigitalOutputs()
 *
 * @param output
 */
void SetCtrlSystemOutput(const SimulinkOutput& output) {
    driver_speaker.Update(output.DI_b_driverSpeaker);
    brake_light.Update(output.DI_b_brakeLightEn);
    // todo: blake - fill in the rest of the outputs
}

/**
 * @brief
 * @note Replaces transmitToAMKMotors()
 *
 * @param output
 * @todo sam - see AMKMotor.Transmit()
 */
void TransmitToAMKMotors(const SimulinkOutput& output) {
    motor_left.Transmit(nullptr);
    motor_right.Transmit(nullptr);
}

/**
 * @brief
 * @note Replaces transmitToBMS()
 *
 * @param output
 */
void TransmitToBMS(const SimulinkOutput& output) {
    // @todo - sam construct a message from the simulink output and send to
    // BMS. You should create an app-level object to manage the BMS CAN
    // peripheral.
}

/**
 * @brief Update the status light
 * @note Replaces setPWMOutputs()
 *
 */
void UpdateStatusLight() {
    /* to do -> what are we actually doing? I thought the status light had 3
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
        simulink_output = ControlSystem::Update(simulink_input);
        SetCtrlSystemOutput(simulink_output);
        TransmitToAMKMotors(simulink_output);
        TransmitToBMS(simulink_output);

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
    ControlSystem::Initialize();

    os::InitializeKernel();

    os::StartKernel();

    while (true) continue;  // logic is handled by OS tasks

    return 0;
}