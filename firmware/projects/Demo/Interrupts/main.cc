#include "bindings.h"
#include <iostream> 


//toggle variable
volatile bool bindings::toggle = false;

//app level function that takes care of the interupt handler
namespace bindings {  // Define AppLevelFunction within the bindings namespace
    void AppLevelFunction() {
        std::cout << "Toggle Switch is Pressed \n";
        bindings::toggle = !bindings::toggle;
    }
}

int main() {
    bindings::Initialize();
    

    while (true) {
        bindings:: setup_interupt();

        //toggling between the high and low once toggle switch has been activated
        if (bindings::toggle){
            bindings::indicator.SetHigh();
        } else{
            bindings::indicator.SetLow();
        }
     
        
       
        bindings::DelayMS(1000);
    }

    return 0;
}