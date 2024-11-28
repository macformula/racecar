/// @author Blake Freer
/// @date 2023-11-08

#pragma once

#include <cstdint>

#include "shared/periph/gpio.hpp"
#include "stm32f7xx_hal.h"

namespace mcal::stm32f767::periph {

class DigitalInput : public shared::periph::DigitalInput {
private:
    GPIO_TypeDef* port_;
    uint16_t pin_;

public:
    DigitalInput(GPIO_TypeDef* gpio_port, uint16_t pin)
        : port_(gpio_port), pin_(pin) {}

    bool Read() override {
        return HAL_GPIO_ReadPin(port_, pin_);
    }
};

class DigitalOutput : public shared::periph::DigitalOutput {
private:
    GPIO_TypeDef* port_;
    uint16_t pin_;

public:
    DigitalOutput(GPIO_TypeDef* gpio_port, uint16_t pin)
        : port_(gpio_port), pin_(pin) {}

    void SetHigh() override {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
    }
    void SetLow() override {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
    }
    void Set(bool value) override {
        HAL_GPIO_WritePin(port_, pin_, static_cast<GPIO_PinState>(value));
    }
};

}  // namespace mcal::stm32f767::periph
