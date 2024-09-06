#include "bindings.h"
#include <stdint.h>

int main() {
    bindings::Initialize();

    while (true) {
        uint32_t accel_pedal_pos_1 = bindings::accel_pedal_1.Read();
        uint32_t accel_pedal_pos_2 = bindings::accel_pedal_2.Read();

        // Send over CAN

        bindings::DelayMS(25);
    }
}