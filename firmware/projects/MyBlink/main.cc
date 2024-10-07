#include "bindings.h"

int main() {
    bindings::Initialize();

    while (true) {

        bindings::test.Start();
        bindings::DelayMS(1000);
        bindings::test.SetFrequency(100);
        bindings::DelayMS(1000);
        bindings::test.GetFrequency();
        bindings::DelayMS(1000);
        bindings::test.Stop();
        bindings::DelayMS(1000);
        bindings::test2.Set(3);
        bindings::DelayMS(1000);
        bindings::test2.Set(50);
        bindings::DelayMS(1000);
    }

    return 0;
}

