#include "bindings.h"



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