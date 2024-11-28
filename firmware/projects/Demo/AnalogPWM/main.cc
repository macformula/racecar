#include "bindings.hpp"

using namespace bindings;

/**
 * @brief Tests the PWMOutput class using various frequencies and duty cycles.
 */
void DemoPWM() {
    pwm.Start();
    DelayMS(1000);

    pwm.SetFrequency(100);
    pwm.SetDutyCycle(50.);
    DelayMS(1000);

    pwm.GetFrequency();
    pwm.GetDutyCycle();
    DelayMS(1000);

    pwm.SetFrequency(100);
    DelayMS(1000);

    pwm.GetFrequency();
    DelayMS(1000);

    pwm.SetFrequency(-5);
    pwm.GetFrequency();
    DelayMS(1000);

    pwm.GetFrequency();
    DelayMS(1000);

    pwm.Stop();
}

/**
 * @brief Tests the AnalogOutput class using various voltages.
 */
void DemoAnalog() {
    analog_out.SetVoltage(1.0f);
    DelayMS(1000);
    analog_out.SetVoltage(4.5f);
    DelayMS(1000);
    analog_out.SetVoltage(0.0f);
    DelayMS(1000);
    analog_out.SetVoltage(3.3f);
    DelayMS(1000);
}

int main() {
    Initialize();

    while (true) {
        DemoPWM();
        DelayMS(1000);

        DemoAnalog();
        DelayMS(1000);
    }

    return 0;
}
