#include "infra/gpio/digital_input.h"
#include "infra/gpio/digital_output.h"
#include "infra/pwm/pwm_output.h"
#include "infra/adc/adc_input.h"

/* BEGIN mcal/{board}/mcal.cc */
extern void Initialize();

extern infra::gpio::DigitalInput& btn;
extern infra::gpio::DigitalOutput& led;
extern infra::pwm::PWMOutput& pwm;
extern infra::adc::ADCInput& adc;
/* END mcal/{board}/mcal.cc */

float adcToPwm(uint32_t adc_value) {
	// remap 12 bit adc to 0-100%
	return float(adc_value) / 4095.0f * 100.0f;
}

int main(void) {
	Initialize();

	pwm.Start();

	uint32_t adc_value;

	while (true) {
		
		led.Set(btn.Read());

		adc_value = adc.Read();	
		pwm.SetDutyCycle(adcToPwm(adc_value));

	}
	
	return 0;
}