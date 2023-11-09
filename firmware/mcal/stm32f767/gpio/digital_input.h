/// @author Blake Freer
/// @date 2023-11-08

#ifndef MCAL_STM32F767_GPIO_DIGITAL_INPUT_H_
#define MCAL_STM32F767_GPIO_DIGITAL_INPUT_H_

#include <stdint.h>
#include "infra/gpio/digital_input.h"
#include "stm32f7xx_hal_gpio.h"

namespace mcal {
namespace gpio {

class DigitalInput : public infra::gpio::DigitalInput {

GPIO_TypeDef* port_;
uint16_t pin_;

public:
	DigitalInput(GPIO_TypeDef* gpio_port, uint16_t pin) : port_(gpio_port), pin_(pin) {}

	bool Read() noexcept override {
		return HAL_GPIO_ReadPin(port_, pin_);
	}

};

} // namespace gpio
} // namespace mcal

#endif