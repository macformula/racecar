/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#ifndef TMS_APP_H_
#define TMS_APP_H_

#include <cstdint>

#include <shared/util/data_structures/lookup_table.h>

#include "shared/periph/gpio.h"
#include "shared/periph/adc.h"
#include "shared/periph/pwm.h"

/***************************************************************
	Function definititions (for those that need an mcal binding)
***************************************************************/

void Initialize();


/***************************************************************
	App-level objects
***************************************************************/

template <shared::periph::DigitalInput DigitalInput>
class PushButton {

private:

	DigitalInput btn_;

public:
	PushButton(DigitalInput btn) : btn_(btn) {}

	bool Read() {
		return btn_.Read();
	}

};

template <shared::periph::DigitalOutput DigitalOutput>
class Indicator {

private:
	DigitalOutput led_;

public:
	Indicator(DigitalOutput led) : led_(led) {}

	void SetLight(bool value) {
		led_.Set(value);
	}
};

template <shared::periph::ADCInput ADCInput>
class TempSensor {

private:
	ADCInput adc_;
	

public:
	TempSensor(ADCInput adc) : adc_(adc) {}

	uint32_t Read() {
		return adc_.Read();
	}
};

template <shared::periph::PWMOutput PWMOutput>
class FanContoller {

private:
	PWMOutput pwm_;

public:
	FanContoller(PWMOutput pwm) : pwm_(pwm) {}

	void Set(float value) {
		pwm_.SetDutyCycle(value);
	}

	void StartPWM() {
		pwm_.Start();
	}

};


#endif