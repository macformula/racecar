/// @author Blake Freer
/// @date 2023-11-09

#ifndef SHARED_PERIPH_ADC_H_
#define SHARED_PERIPH_ADC_H_

#include "stdint.h"
#include <concepts>

#include "shared/util/peripheral.h"

namespace shared::periph {

template <typename T>
concept ADCInput = requires(T obj) {
	{ obj.Start() } -> std::same_as<void>;
	{ obj.Read() } -> std::same_as<uint32_t>;

	std::is_base_of_v<util::Peripheral, T>;
};

} // namespace shared::periph

#endif