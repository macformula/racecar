#include "bindings.h"
#include <iostream> 


//toggle variable
volatile bool toggle = false;

void ToggleInterruptHandler() {
    bindings::AppLevelFunction();
}

//app level function that takes care of the interupt handler
namespace bindings {  // Define AppLevelFunction within the bindings namespace
    void AppLevelFunction() {
        std::cout << "Toggle Switch is Pressed" << std::endl;
        toggle = !toggle;
    }
}

int main() {
    bindings::Initialize();

    while (true) {
        //toggling between the high and low once toggle switch has been activated
        if (toggle){
            bindings::indicator.SetHigh();
        } else{
            bindings::indicator.SetLow();
        }
     
        
       
        bindings::DelayMS(1000);
    }

    return 0;
}