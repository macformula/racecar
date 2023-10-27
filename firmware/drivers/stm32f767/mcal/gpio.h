/// @author Blake Freer
/// @date 2023-10-25
/// @note See https://github.com/tomikaa87/stm32-hal-cpp

#ifndef INFRA_MCAL_STM32F767_GPIO_H_
#define INFRA_MCAL_STM32F767_GPIO_H_

#include <cstdint>

#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"
#include "util/disjunction.h"

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

template <uint32_t... _pins>
struct pins {
    static const uint32_t value = util::FlagDisjunction<_pins...>::value;
};

struct all_pins {
    static const uint32_t value = GPIO_PIN_All;
};

namespace mode {
enum modes {
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
}  // namespace mode

namespace pull {
enum pull_mode {
    nopull = GPIO_NOPULL,
    up = GPIO_PULLUP,
    down = GPIO_PULLDOWN,
};
}  // namespace pull

namespace speed {
enum speed_mode {
    low = GPIO_SPEED_FREQ_LOW,
    medium = GPIO_SPEED_FREQ_MEDIUM,
    high = GPIO_SPEED_FREQ_HIGH,
    very_high = GPIO_SPEED_FREQ_VERY_HIGH,
};
}  // namespace speed

template <typename _gpio,
		  typename _pin_nums,
		  mode::modes _mode,
		  pull::pull_mode _pull = pull::nopull,
		  speed::speed_mode _speed = speed::low>
struct pin {
    static const auto pin_value_ = _pin_nums::value;
    static const auto mode_ = _mode;
    static const auto speed_ = _speed;
    static const auto pull_ = _pull;
    using gpio_ = _gpio;

    inline static void init() {
        GPIO_InitTypeDef gpio_init;
        gpio_init.Mode = mode_;
        gpio_init.Pin = pin_value_;
        gpio_init.Speed = speed_;
        gpio_init.Pull = pull_;

        gpio_::enable_clock();
        HAL_GPIO_Init(gpio_::get(), &gpio_init);
    }

    inline static void deinit() {
		HAL_GPIO_DeInit(gpio_::get(), pin_value_);
	}

    inline static bool lock() {
        return HAL_GPIO_LockPin(gpio_::get(), pin_value_) == HAL_OK;
    }
	
	/// may need to move back to output_pin
	inline static void set() {
		static_assert(mode_ == mode::output_od || mode_ == mode::output_pp,
			"pin must be configured as an output");
		HAL_GPIO_WritePin(gpio_::get(), pin_value_, GPIO_PIN_SET);
	}
	
	/// may need to move back to input_pin
	inline static bool read() {
		return HAL_GPIO_ReadPin(gpio_::get(), pin_value_) == GPIO_PIN_SET;
	}
};

template <typename pin>
struct output_pin {
	using pin_ = pin;
	inline static void init() {
		static_assert(pin_::mode_ == mode::output_od || pin_::mode_ == mode::output_pp,
			"pin must be configured as an output");
		
		pin_::init();
	}

	inline static void reset() {
		HAL_GPIO_WritePin(pin_::gpio_::get(), pin_::pin_value_, GPIO_PIN_RESET);
	}

	inline static void toggle () {
		HAL_GPIO_TogglePin(pin_::gpio_::get(), pin::pin_value_);
	}

	inline static void set() {
		static_assert(pin_::mode_ == mode::output_od || pin_::mode_ == mode::output_pp,
			"pin must be configured as an output");
		HAL_GPIO_WritePin(pin_::gpio_::get(), pin_::pin_value_, GPIO_PIN_SET);
	}
};

template <typename pin>
struct input_pin {
	using pin_ = pin;
	inline static void init() {
		static_assert(pin_::mode == mode::input,
			"pin must be configured as an input");
		pin_::init();
	}
	inline static bool read() {
		return HAL_GPIO_ReadPin(pin_::gpio_::get(), pin_::pin_value_) == GPIO_PIN_SET;
	}
};


}  // namespace gpio

}  // namespace mcal

#endif