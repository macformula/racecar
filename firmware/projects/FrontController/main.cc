/// @author Blake Freer
/// @date 2024-02-24

#include "app.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "simulink.h"

namespace bindings {
extern shared::periph::DigitalOutput& driver_speaker;
extern shared::periph::DigitalOutput& brake_light;
extern shared::periph::ADCInput& accel_pedal_1;
extern shared::periph::ADCInput& accel_pedal_2;
extern shared::periph::ADCInput& brake_pedal;
extern shared::periph::ADCInput& steering_wheel;
extern shared::periph::DigitalInput& driver_button;

extern void Initialize();
}  // namespace bindings

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
    Globals
***************************************************************/

SimulinkInput simulink_buffer_input;
SimulinkOutput simulink_buffer_output;

/***************************************************************
    Task Functions
***************************************************************/

/**
 * @brief Replaces readFromAdc
 */
void TakeInput_Task() {
    simulink_buffer_input.DI_b_DriverButton = driver_button.Read();
    simulink_buffer_input.DI_V_AccelPedalPos1 = accel_pedal_1.Update();
    simulink_buffer_input.DI_V_AccelPedalPos2 = accel_pedal_2.Update();
    simulink_buffer_input.DI_V_BrakePedalPos = brake_pedal.Update();
    simulink_buffer_input.DI_V_SteeringAngle = steering_wheel.Update();
}

/**
 * @brief Copy values to the simulink input struct.
 */
void SetControlSystemInputs_Task() {
    simulink_buffer_output = ControlSystem::Update(simulink_buffer_input);
}

void ControllerAutogenStep_Task() {}

void GetControlSystemOutputs_Task() {
    driver_speaker.state = simulink_buffer_output.DI_b_driverSpeaker;
    brake_light.state = simulink_buffer_output.DI_b_brakeLightEn;
}

void TransmitToAMKMotors_Task() {
    motor_left.Transmit(nullptr);
    motor_right.Transmit(nullptr);
}

void TransmitToBMS_Task() {}

void SetDigitalOutputs_Task() {
    driver_speaker.Update();
    brake_light.Update();
}

void SetPWMOutputs_Task() {}

int main(void) {
    bindings::Initialize();
    ControlSystem::Initialize();

    while (1) {
    }

    return 0;
}