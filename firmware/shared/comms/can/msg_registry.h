/// @author Samuel Parent
/// @date 2024-03-01

#pragma once

#include "shared/comms/can/can_msg.h"

namespace shared::comms::can {

class MsgRegistry {
public:
    virtual bool SetMessage(const RawCanMsg&) = 0;
    virtual bool GetMessage(CanRxMsg&) = 0;
};

}  // namespace shared::comms::can