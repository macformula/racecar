/// @author Blake Freer
/// @date 2023-11-08

#ifndef MCAL_STM32F767_GPIO_DIGITAL_OUTPUT_H_
#define MCAL_STM32F767_GPIO_DIGITAL_OUTPUT_H_

#include <stdint.h>
#include "stm32f7xx_hal_gpio.h"
#include "infra/gpio/digital_output.h"

namespace mcal {
namespace gpio {

class DigitalOutput : public infra::gpio::DigitalOutput {

GPIO_TypeDef* port_;
uint16_t pin_;

public:
	DigitalOutput(GPIO_TypeDef* gpio_port, uint16_t pin) : port_(gpio_port), pin_(pin) {}
	
	void SetHigh() noexcept override {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
	}
	void SetLow() noexcept override {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
	}
	void Set(bool value) noexcept override {
		HAL_GPIO_WritePin(port_, pin_, GPIO_PinState(value));
	}
};

} // namespace gpio
} // namespace mcal

#endif