#include "bindings.h"
#include <iostream> 


//toggle variable
volatile bool toggle = false;

void AppLevelFunction();

void ToggleInterruptHandler() {
    AppLevelFunction();
}

//app level function that takes care of the interupt handler
void AppLevelFunction() {
    std::cout << "Toggle Switch is Pressed" << std::endl;
    toggle = !toggle;
}

int main() {
    bindings::Initialize();

    while (true) {
        //toggling between the high and low once toggle switch has been activated
        bindings::indicator.Set(toggle);
        bindings::DelayMS(1000);
    }

    return 0;
}