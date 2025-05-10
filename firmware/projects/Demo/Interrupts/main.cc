#include "bindings.hpp"

volatile bool toggle = false;

void AppInterruptHandler(int) {
    // This is called by the a platform's interrupt mechanism. See
    // platforms/cli/bindings.cc
    toggle = !toggle;
    bindings::indicator.Set(toggle);
}

int main() {
    bindings::Initialize();
    bindings::indicator.SetLow();

    // Nothing happens in the loop. All behaviour occurs in the
    // interrupt handler function.
    while (true) continue;

    return 0;
}