/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

#include "TMS/mcal/stm32f767/bindings.h"

const int kTempSensorCount = 6;
extern mcal::periph::ADCInput temp_sensor_adc[kTempSensorCount];

TempSensor<

extern TempSensor<mcal::periph::ADCInput> temp_sensors[kTempSensorCount];

/**
 * @brief Replaces HAL_TIM_PeriodElapsedCallback
*/
void TaskUpdate(){
    
}

int main(void) {
    Initialize();

    fanController.StartPWM();

    uint32_t tempValue;

    while (true) {}

    return 0;
}