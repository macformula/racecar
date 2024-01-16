/// @author Samuel Parent
/// @date 2024-01-06

#ifndef SHARED_PERIPH_ADC_H_
#define SHARED_PERIPH_ADC_H_

#include <cstdint>
#include <concepts>

#include "shared/util/peripheral.h"
#include "shared/comms/can/raw_can_msg.h"

namespace shared::periph {

template <typename T>
concept Can = requires(T obj, shared::comms::can::raw_can_msg can_rx_msgs[], const shared::comms::can::raw_can_msg& can_tx_msg) {
	{ obj.Send(can_tx_msg) } -> std::same_as<void>;
	{ obj.ReadQueue(can_rx_msgs) } -> std::same_as<void>;

	std::is_base_of_v<util::Peripheral, T>;
};

} // namespace shared::periph

#endif