/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

#include "TMS/mcal/stm32f767/bindings.h"

namespace bindings {
    extern mcal::periph::DigitalInput button_di;
    extern mcal::periph::DigitalOutput light_do;
    extern mcal::periph::PWMOutput fan_controller_pwm;
    extern mcal::periph::ADCInput temp_sensor_adc;
} // namespace bindings

PushButton button{bindings::button_di};
Indicator light{bindings::light_do};
FanContoller fanController{bindings::fan_controller_pwm};
TempSensor tempSensor{bindings::temp_sensor_adc};

float tempToPWM(uint32_t adc_value) {
    // remap 12 bit adc to 0-100%
    return float(adc_value) / 4095.0f * 100.0f;
}

int main(void) {
    Initialize();

    fanController.StartPWM();

    uint32_t tempValue;

    while (true) {
        button.Read();
        light.SetLight(button.Read());

        tempValue = tempSensor.Read();
        fanController.Set(tempToPWM(tempValue));
    }

    return 0;
}