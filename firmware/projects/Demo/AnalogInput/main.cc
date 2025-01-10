#include "bindings.hpp"


int main(){
    bindings::Init();

    while (true) {
        bindings::analog_input.Read();
    }
    return 0;
}