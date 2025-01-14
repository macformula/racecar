#include "bindings.hpp"

int main() {
    bindings::Init();

    while (true) {
        bindings::analog_input.ReadVoltage();
    }
    return 0;
}