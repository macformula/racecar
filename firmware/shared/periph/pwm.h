/// @author Blake Freer
/// @date 2023-11-09

#ifndef SHARED_PERIPH_PWM_H_
#define SHARED_PERIPH_PWM_H_

#include <concepts>
#include "shared/util/peripheral.h"

namespace shared::periph {

template <typename T>
concept PWMOutput = requires(T obj, float duty_cycle) {
	{ obj.Start() } -> std::same_as<void>;
	{ obj.Stop() } -> std::same_as<void>;
	{ obj.SetDutyCycle(duty_cycle) } -> std::same_as<void>;
	{ obj.GetDutyCycle() } -> std::same_as<float>;

	// force the implementation to inherit from util::Peripheral
	std::is_base_of_v<util::Peripheral, T>;
};

} // namespace shared::periph

#endif