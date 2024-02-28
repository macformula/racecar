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

struct SimulinkInput simulink_buffer_input;

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
    auto controller_autogen_U = simulink_buffer_input;
}

void ControllerAutogenStep_Task() {}

void GetControlSystemOutputs_Task() {
    driver_speaker.state = false;  // replace with ctrl output
    brake_light.state = false;     // replace with ctrl output
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

    while (1) {
    }

    return 0;
}