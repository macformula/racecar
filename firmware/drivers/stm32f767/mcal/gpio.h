/// @author Blake Freer
/// @date 2023-10-25
/// @note See https://github.com/tomikaa87/stm32-hal-cpp

#ifndef DRIVERS_STM32F767_MCAL_GPIO_H_
#define DRIVERS_STM32F767_MCAL_GPIO_H_

#include <cstdint>

#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"

namespace mcal {
namespace gpio {

namespace port {

#define GPIO_STRUCT(_PORT, _NAME)                           \
    struct _NAME {                                          \
        static inline GPIO_TypeDef* get() { return _PORT; } \
        static inline void enable_clock() {                 \
            __HAL_RCC_##_PORT##_CLK_ENABLE();               \
        }                                                   \
        static inline void disable_clock() {                \
            __HAL_RCC_##_PORT##_CLK_DISABLE();              \
        }                                                   \
    };

GPIO_STRUCT(GPIOA, a)
GPIO_STRUCT(GPIOB, b)
GPIO_STRUCT(GPIOC, c)
GPIO_STRUCT(GPIOD, d)
GPIO_STRUCT(GPIOE, e)
GPIO_STRUCT(GPIOF, f)
GPIO_STRUCT(GPIOG, g)
GPIO_STRUCT(GPIOH, h)
GPIO_STRUCT(GPIOI, i)
GPIO_STRUCT(GPIOJ, j)
GPIO_STRUCT(GPIOK, k)

}  // namespace port

/// @brief Enumerated constants for the GPIO pin numbers
enum class pin_num {
	p0 = GPIO_PIN_0,
	p1 = GPIO_PIN_1,
	p2 = GPIO_PIN_2,
	p3 = GPIO_PIN_3,
	p4 = GPIO_PIN_4,
	p5 = GPIO_PIN_5,
	p6 = GPIO_PIN_6,
	p7 = GPIO_PIN_7,
	p8 = GPIO_PIN_8,
	p9 = GPIO_PIN_9,
	p10 = GPIO_PIN_10,
	p11 = GPIO_PIN_11,
	p12 = GPIO_PIN_12,
	p13 = GPIO_PIN_13,
	p14 = GPIO_PIN_14,
	p15 = GPIO_PIN_15,
	all_pins = GPIO_PIN_All
};

/// @brief All available STM32F7 GPIO modes
enum class mode {
    input = GPIO_MODE_INPUT,
    output_od = GPIO_MODE_OUTPUT_OD,
    output_pp = GPIO_MODE_OUTPUT_PP,
    af_od = GPIO_MODE_AF_OD,
    af_pp = GPIO_MODE_AF_PP,
    analog = GPIO_MODE_ANALOG,
    it_rising = GPIO_MODE_IT_RISING,
    it_falling = GPIO_MODE_IT_FALLING,
    it_both_edges = GPIO_MODE_IT_RISING_FALLING,
    evt_rising = GPIO_MODE_EVT_RISING,
    evt_falling = GPIO_MODE_EVT_FALLING,
    evt_both_edges = GPIO_MODE_EVT_RISING_FALLING,
};

/// @brief Pull Up/Down modes
enum class pull {
    nopull = GPIO_NOPULL,
    up = GPIO_PULLUP,
    down = GPIO_PULLDOWN,
};

/// @brief GPIO port speed settings
enum class speed {
    low = GPIO_SPEED_FREQ_LOW,
    medium = GPIO_SPEED_FREQ_MEDIUM,
    high = GPIO_SPEED_FREQ_HIGH,
    very_high = GPIO_SPEED_FREQ_VERY_HIGH,
};

/// @brief STM32F767 GPIO Pin wrapper
/// @tparam _gpio Select GPIO port from `mcal::gpio::port::`
/// @tparam _pin_num Pin number from `mcal::gpio::pin_num::` 
/// @tparam _mode GPIO pin mode from `mcal::gpio::mode::`
/// @tparam _pull Push Up/Down resistor setting from `mcal::gpio::pull::`
/// @tparam _speed Port Speed settings from `mcal::gpio::speed::`
template <typename _gpio_port,
		  pin_num _pin_num,
		  mode _mode,
		  pull _pull = pull::nopull,
		  speed _speed = speed::low>
struct pin {
    static const auto pin_num_ = _pin_num;
    static const auto mode_ = _mode;
    static const auto speed_ = _speed;
    static const auto pull_ = _pull;
    using gpio_ = _gpio_port;

    inline static void init() {
        GPIO_InitTypeDef gpio_init;
        gpio_init.Mode = static_cast<uint32_t>(mode_);
        gpio_init.Pin = static_cast<uint32_t>(pin_num_);
        gpio_init.Speed = static_cast<uint32_t>(speed_);
        gpio_init.Pull = static_cast<uint32_t>(pull_);

        gpio_::enable_clock();
        HAL_GPIO_Init(gpio_::get(), &gpio_init);
    }

    inline static void deinit() {
		HAL_GPIO_DeInit(gpio_::get(), static_cast<uint32_t>(pin_num_));
	}

    inline static bool lock() {
        return HAL_GPIO_LockPin(gpio_::get(), static_cast<uint32_t>(pin_num_)) == HAL_OK;
    }

	/// @brief Set output to HIGH
	/// @note Only available with output modes
	inline static void set() {
		static_assert(mode_ == mode::output_od || mode_ == mode::output_pp,
			"pin must be configured as an output");
		HAL_GPIO_WritePin(gpio_::get(), static_cast<uint32_t>(pin_num_), GPIO_PIN_SET);
	}

	/// @brief Set output to LOW
	/// @note Only available with output modes
	inline static void reset() {
		static_assert(mode_ == mode::output_od || mode_ == mode::output_pp,
			"pin must be configured as an output");
		HAL_GPIO_WritePin(gpio_::get(), static_cast<uint32_t>(pin_num_), GPIO_PIN_RESET);
	}
	
	inline static bool read() {
		return HAL_GPIO_ReadPin(gpio_::get(), static_cast<uint32_t>(pin_num_)) == GPIO_PIN_SET;
	}
};

}  // namespace gpio

}  // namespace mcal

#endif