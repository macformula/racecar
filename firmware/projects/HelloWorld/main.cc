/// @author Blake Freer
/// @date 2023-12-25

#include "app.h"
#include "bindings.h"

namespace bindings {
extern mcal::periph::DigitalInput button_di;
extern mcal::periph::DigitalOutput indicator_do;
extern void Initialize();
}  // namespace bindings

Button button{bindings::button_di};
Indicator indicator{bindings::indicator_do};

bool btn_value;

int main(void) {
    bindings::Initialize();

    while (1) {
        btn_value = button.Read();
        indicator.Set(btn_value);
    }

    return 0;
}