/// @author Blake Freer
/// @date 2023-12-25

#include "app.h"
#include "bindings.h"

namespace bindings {
extern mcal::periph::DigitalInput button_di;
extern mcal::periph::DigitalOutput indicator_do;
}  // namespace bindings

extern void Initialize();

Button button{bindings::button_di};
Indicator indicator{bindings::indicator_do};

bool btn_value = true;

int main(void) {
    Initialize();

    while (1) {
        HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_RESET);
        HAL_Delay(200);
    }
    // while (true) {
    //     for (int i = 0; i < 10000; i++) {
    //         HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_RESET);
    //     }
    //     for (int i = 0; i < 10000; i++) {
    //         HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_SET);
    //     }
    // }

    return 0;
}