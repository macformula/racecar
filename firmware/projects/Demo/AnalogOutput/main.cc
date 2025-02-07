#include "bindings.hpp"

int main() {
    bindings::Init();

    float output_voltage = 0;

    while (true) {
        auto button_pressed = bindings::button.Read();
        if (button_pressed) {
            output_voltage += 1.0F;
            if (output_voltage >= 3.3F) {
                output_voltage = 0.0F;
            }

            bindings::analog_output.SetVoltage(output_voltage);
        }

        bindings::DelayMs(5);

        float volts = bindings::analog_input.ReadVoltage();

        bool red = false, green = false, blue = false;

        if (volts <= 0.5) {
            red = true;
        } else if (volts <= 1.5) {
            blue = true;
        } else if (volts <= 2.5) {
            green = true;
        } else {
            red = true;
            blue = true;
            green = true;
        }

        bindings::red_led.Set(red);
        bindings::blue_led.Set(blue);
        bindings::green_led.Set(green);
    }
    return 0;
}