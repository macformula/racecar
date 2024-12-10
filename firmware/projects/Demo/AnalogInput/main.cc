#include "bindings.h"


int main(){
    bindings::Init();

    while (true) {
        bindings::analog_input.Read();
    }
    return 0;
}