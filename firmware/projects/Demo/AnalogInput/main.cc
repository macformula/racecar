#include "bindings.hpp"

int main() {
    bindings::Init();

    while (true) {
        float volts = bindings::analog_input.ReadVoltage();
        if (volts <= 1) {
            bindings::red_Led.SetHigh();
            bindings::green_Led.SetLow();
            bindings::blue_Led.SetLow();
        } else if (volts <= 2) {
            bindings::red_Led.SetLow();
            bindings::green_Led.SetLow();
            bindings::blue_Led.SetHigh();
        } else if (volts <= 3) {
            bindings::red_Led.SetLow();
            bindings::green_Led.SetHigh();
            bindings::blue_Led.SetLow();
        } else {
            bindings::red_Led.SetHigh();
            bindings::green_Led.SetHigh();
            bindings::blue_Led.SetHigh();
        }
    }
    return 0;
}