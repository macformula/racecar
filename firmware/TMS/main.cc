/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

#include "TMS/mcal/stm32f767/bindings.h"

extern PushButton<mcal::periph::DigitalInput> button;
extern Indicator<mcal::periph::DigitalOutput> light;
extern FanContoller<mcal::periph::PWMOutput> fanController;
extern TempSensor<mcal::periph::ADCInput> tempSensor;

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