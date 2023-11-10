/// @author Blake Freer
/// @date 2023-11-08


// CubeMX imports
#include "main.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"

#include "infra/gpio/digital_input.h"
#include "infra/gpio/digital_output.h"
#include "infra/pwm/pwm_output.h"
#include "infra/adc/adc_input.h"

#include "mcal/stm32f767/gpio/digital_input.h"
#include "mcal/stm32f767/gpio/digital_output.h"
#include "mcal/stm32f767/pwm/pwm_output.h"
#include "mcal/stm32f767/adc/adc_input.h"

// Create microcontroller level objects
const infra::gpio::DigitalInput& btn = mcal::gpio::DigitalInput(ButtonPin_GPIO_Port, ButtonPin_Pin);
const infra::gpio::DigitalOutput& led = mcal::gpio::DigitalOutput(LedPin_GPIO_Port, LedPin_Pin);
const infra::pwm::PWMOutput& pwm = mcal::pwm::PWMOutput(&htim1, TIM_CHANNEL_1);
const infra::adc::ADCInput& adc = mcal::adc::ADCInput(&hadc1);

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