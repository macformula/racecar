#include <iostream>

#include "bindings.hpp"

int main() {
    bindings::Initialize();

    while (true) {
        std::cout << "Hello World!" << std::endl;
        bindings::indicator.SetHigh();
        bindings::DelayMS(500);
        bindings::indicator.SetLow();
        bindings::DelayMS(500);
    }

    return 0;
}