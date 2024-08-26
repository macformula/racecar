#include "bindings.h"

int main() {
    bindings::Initialize();

    while (true) {
        bindings::indicator.SetHigh();
        bindings::DelayMS(1000);
        bindings::indicator.SetLow();
        bindings::DelayMS(1000);
    }

    return 0;
}