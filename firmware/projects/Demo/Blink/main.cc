#include "bindings.h"

int add(int a, int b) {
    return a + b;
}

#ifdef MAIN_PROGRAM
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
#endif