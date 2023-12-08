#include "bindings.h"

// CubeMX imports
#include "TMS/app.h"
#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// Create microcontroller level objects
namespace bindings {
mcal::periph::DigitalInput button{ButtonPin_GPIO_Port, ButtonPin_Pin};
mcal::periph::DigitalOutput light{LedPin_GPIO_Port, LedPin_Pin};
mcal::periph::ADCInput tempSensor{&hadc1};
mcal::periph::PWMOutput fanController{&htim1, TIM_CHANNEL_1};
} // namespace bindings

extern "C" {
void SystemClock_Config();
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_ADC1_Init();
    MX_GPIO_Init();
    MX_TIM1_Init();
}