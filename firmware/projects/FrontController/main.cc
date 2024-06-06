/// @author Blake Freer
/// @date 2024-02-24

#include <sys/_stdint.h>

#include "app.h"
#include "bindings.h"
#include "shared/os/os.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "simulink.h"

/***************************************************************
    Objects
***************************************************************/

Speaker driver_speaker{bindings::driver_speaker};
BrakeLight brake_light{bindings::brake_light};

Pedal accel_pedal_1{bindings::accel_pedal_1};
Pedal accel_pedal_2{bindings::accel_pedal_2};
Pedal brake_pedal{bindings::brake_pedal};
SteeringWheel steering_wheel{bindings::steering_wheel};
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

/**
 * @brief Read the inputs from all devices to send to the control system.
 * @note Replaces readFromAdc()
 *
 * @return SimulinkInput
 */
SimulinkInput ReadCtrlSystemInput() {
    SimulinkInput input;

    input.DI_b_DriverButton = driver_button.Read();
    input.DI_V_AccelPedalPos1 = accel_pedal_1.Update();
    input.DI_V_AccelPedalPos2 = accel_pedal_2.Update();
    input.DI_V_BrakePedalPos = brake_pedal.Update();
    input.DI_V_SteeringAngle = steering_wheel.Update();
    // todo: blake - fill in the rest of the inputs

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
    // @todo - sam construct a message from the simulink output and send to BMS.
    // You should create an app-level object to manage the BMS CAN peripheral.
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