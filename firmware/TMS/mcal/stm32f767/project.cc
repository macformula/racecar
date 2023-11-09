/// @author Blake Freer
/// @date 2023-11-08


#include "gpio.h"
#include "adc.h"
#include "main.h"

#include "infra/gpio/digital_input.h"
#include "infra/gpio/digital_output.h"

#include "mcal/stm32f767/gpio/digital_input.h"
#include "mcal/stm32f767/gpio/digital_output.h"


// // Create microcontroller level objects
mcal::gpio::DigitalInput mcal_button(ButtonPin_GPIO_Port, ButtonPin_Pin);
mcal::gpio::DigitalOutput mcal_led(LedPin_GPIO_Port, LedPin_Pin);

// // Bind to the infra-level parent type
infra::gpio::DigitalInput& btn = mcal_button;
infra::gpio::DigitalOutput& led = mcal_led;

extern "C" {
	void SystemClock_Config();
}

void Initialize() {
	SystemClock_Config();
	HAL_Init();
	MX_ADC1_Init();
	MX_GPIO_Init();
}