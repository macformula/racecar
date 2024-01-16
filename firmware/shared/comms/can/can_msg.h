/// @author Samuel Parent
/// @date 2024-01-06

#ifndef SHARED_COMMS_CAN_CAN_MSG_H_
#define SHARED_COMMS_CAN_CAN_MSG_H_

#include <cstdint>
#include <concepts>

#include "shared/comms/can/raw_can_msg.h"

namespace shared::comms::can {

template <typename T>
concept CanMsg = requires(T obj, const raw_can_msg& msgs) {
	{ obj.Pack(msgs) } -> std::same_as<void>;
	{ obj.Unpack(msgs) } -> std::same_as<void>;
};

} // namespace shared::comms::can

#endif // SHARED_COMMS_CAN_CAN_MSG_H_