/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <cstdint>

#include "shared/comms/can/raw_can_msg.h"

namespace shared::comms::can {

class DemoCanVehMsgRegistry;

class CanMsg {};

class CanRxMsg : public CanMsg {
public:
    virtual void Clone(CanRxMsg&) = 0;
    virtual void Unpack(const RawCanMsg&) = 0;
    virtual CanId Id() = 0;
};

class CanTxMsg : public CanMsg {
private:
    virtual void Pack(RawCanMsg&) = 0;

    friend class CanBus;
};

}  // namespace shared::comms::can
