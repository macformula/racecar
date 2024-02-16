/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <cstdint>
#include <concepts>

#include "shared/util/peripheral.h"
#include "shared/comms/can/raw_can_msg.h"

namespace shared::periph {

class CanBase : public util::Peripheral {
public:
	virtual void Send(const RawCanMsg&) = 0;
	virtual void ReadQueue(RawCanMsg[]) = 0;
};

} // namespace shared::periph