/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

#include "TMS/mcal/stm32f767/bindings.h"

namespace bindings {
    extern mcal::periph::PWMOutput fan_controller_pwm;
    extern mcal::periph::ADCInput temp_sensor_adc;
} // namespace bindings

FanContoller fan_controller{bindings::fan_controller_pwm};
TempSensor temp_sensor{bindings::temp_sensor_adc};

float tempToPWM(uint32_t adc_value) {
    // remap 12 bit adc to 0-100%
    return float(adc_value) / 4095.0f * 100.0f;
}

int main(void) {
    Initialize();

    fan_controller.StartPWM();

    while (true) {}

    return 0;
}