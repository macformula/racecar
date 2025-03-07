#include "bindings.hpp"

int main() {
    bindings::Init();

    bindings::red_led.SetHigh();
    bindings::blue_led.SetLow();

    bindings::DelayMS(5000);

    bindings::blue_led.SetHigh();
    bindings::red_led.SetLow();

    while (true) {
        if (bindings::reset_button.Read()) {
            bindings::SoftwareReset();
        }
    }

    return 0;
}