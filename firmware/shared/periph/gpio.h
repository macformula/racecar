/// @author Blake Freer
/// @date 2023-11-08

#ifndef SHARED_PERIPH_GPIO_H_
#define SHARED_PERIPH_GPIO_H_

#include <concepts>
#include "shared/util/peripheral.h"

namespace shared::periph {

template <typename T>
concept DigitalInput = requires(T obj) {
	{ obj.Read() } -> std::same_as<bool>;

	std::is_base_of_v<util::Peripheral, T>;
};

template <typename T>
concept DigitalOutput = requires(T obj, bool value) {
	{ obj.SetHigh() } -> std::same_as<void>;
	{ obj.SetLow() } -> std::same_as<void>;
	{ obj.Set(value) } -> std::same_as<void>;

	std::is_base_of_v<util::Peripheral, T>;
};

} // namespace shared::periph

#endif