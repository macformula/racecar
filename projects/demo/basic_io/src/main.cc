/// @author Blake Freer
/// @date 2023-12-25

#include "bindings.hpp"

bool btn_value;

int main(void) {
    bindings::Initialize();

    while (1) {
        btn_value = bindings::button_di.Read();
        bindings::indicator_do.Set(btn_value);
    }

    return 0;
}