#include "bindings.h"

// CubeMX imports
#include "TMS/app.h"
#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// Create microcontroller level objects
PushButton button{mcal::periph::DigitalInput(ButtonPin_GPIO_Port, ButtonPin_Pin)};
Indicator light{mcal::periph::DigitalOutput(LedPin_GPIO_Port, LedPin_Pin)};
TempSensor tempSensor{mcal::periph::ADCInput(&hadc1)};
FanContoller fanController{mcal::periph::PWMOutput(&htim1, TIM_CHANNEL_1)};

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