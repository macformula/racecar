/// @author Blake Freer
/// @date 2023-11-08


// CubeMX imports
#include "main.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"

#include "TMS/app.h"

#include "mcal/stm32f767/periph/gpio.h"
#include "mcal/stm32f767/periph/pwm.h"
#include "mcal/stm32f767/periph/adc.h"

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