/// @author Blake Freer
/// @date 2023-11-08

#ifndef MCAL_STM32F767_PERIPH_GPIO_H_
#define MCAL_STM32F767_PERIPH_GPIO_H_

#include <cstdint>
#include "stm32f7xx_hal.h"

#include "shared/util/peripheral.h"

namespace mcal::periph {

class DigitalInput : public shared::util::Peripheral {

private:
	GPIO_TypeDef* port_;
	uint16_t pin_;

public:
	DigitalInput(GPIO_TypeDef* gpio_port, uint16_t pin) : port_(gpio_port), pin_(pin) {}

	bool Read() {
		return HAL_GPIO_ReadPin(port_, pin_);
	}

};

class DigitalOutput : public shared::util::Peripheral {

private:
	GPIO_TypeDef* port_;
	uint16_t pin_;

public:
	DigitalOutput(GPIO_TypeDef* gpio_port, uint16_t pin) : port_(gpio_port), pin_(pin) {}
	
	void SetHigh() {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
	}
	void SetLow() {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
	}
	void Set(bool value) {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PinState(value));
	}
};

} // namespace mcal::periph

#endif