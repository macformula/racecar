/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <cstdint>

#include "shared/comms/can/raw_can_msg.h"

namespace shared::comms::can {

class CanMsg {};

class CanRxMsg : public CanMsg {
private:
	virtual Unpack(const RawCanMsg&) = 0;
	virtual uint32_t Id() = 0;
};

class CanTxMsg : public CanMsg {
private:
	virtual void Pack(RawCanMsg&) = 0;
};

} // namespace shared::comms::can

#endif // SHARED_COMMS_CAN_CAN_MSG_H_