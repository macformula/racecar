#include "bindings.hpp"

int main() {
    bindings::Init();

    bool red = false, green = false, blue = false;

    while (true) {
        float volts = bindings::analog_input.ReadVoltage();

        if (volts <= 1) {
            red = true;
        } else if (volts <= 2) {
            blue = true;
        } else if (volts <= 3) {
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