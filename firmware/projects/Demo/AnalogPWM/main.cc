#include "bindings.h"

int main() {
    bindings::Initialize();

    while (true) {

        // tests the PWMOutput class using various frequencies
        bindings::pwm.Start();
        bindings::DelayMS(1000);
        bindings::pwm.SetFrequency(0);
        bindings::DelayMS(1000);
        bindings::pwm.GetFrequency();
        bindings::DelayMS(1000);
        bindings::pwm.SetFrequency(100);
        bindings::DelayMS(1000);
        bindings::pwm.GetFrequency();
        bindings::DelayMS(1000);
        bindings::pwm.SetFrequency(-5);
        bindings::DelayMS(1000);
        bindings::pwm.GetFrequency();
        bindings::DelayMS(1000);
        bindings::pwm.Stop();

        // tests the AnalogOutput class using various voltages
        bindings::DelayMS(1000);
        bindings::analog_out.SetVoltage(3);
        bindings::DelayMS(1000);
        bindings::analog_out.SetVoltage(50);
        bindings::DelayMS(1000);

    }

    return 0;
}

