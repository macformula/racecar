#include "bindings.hpp"
#include "devices.hpp"

int main() {
    bindings::Init();

    // Start both pots at midpoint (127)
    uint8_t pot0_position = 127;
    uint8_t pot1_position = 127;

    device::digital_pot0.SetPosition(pot0_position);
    device::digital_pot1.SetPosition(pot1_position);

    while (true) {
        auto button0_pressed = bindings::button_increment_pot0.Read();
        auto button1_pressed = bindings::button_increment_pot1.Read();
        auto button_terminal0_pressed =
            bindings::button_terminal_connection_pot0.Read();
        auto button_terminal1_pressed =
            bindings::button_terminal_connection_pot1.Read();

        if (button0_pressed) {
            // Increment pot0, decrement pot1
            if (pot0_position < 255) {
                pot0_position++;
            }
            if (pot1_position > 0) {
                pot1_position--;
            }

            device::digital_pot0.SetPosition(pot0_position);
            device::digital_pot1.SetPosition(pot1_position);

            bindings::green_led.Set(true);
        } else {
            bindings::green_led.Set(false);
        }

        if (button1_pressed) {
            // Increment pot1, decrement pot0
            if (pot1_position < 255) {
                pot1_position++;
            }
            if (pot0_position > 0) {
                pot0_position--;
            }

            device::digital_pot0.SetPosition(pot0_position);
            device::digital_pot1.SetPosition(pot1_position);

            bindings::red_led.Set(true);
        } else {
            bindings::red_led.Set(false);
        }

        // Handle terminal connections
        if (button_terminal0_pressed) {
            device::digital_pot0.SetTerminalConnections(false, false, true);
        } else {
            device::digital_pot0.SetTerminalConnections(true, true, true);
        }

        if (button_terminal1_pressed) {
            device::digital_pot1.SetTerminalConnections(false, false, true);
        } else {
            device::digital_pot1.SetTerminalConnections(true, true, true);
        }

        bindings::DelayMs(20);
    }
    return 0;
}