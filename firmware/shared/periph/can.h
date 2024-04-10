/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <cstdint>

#include "shared/comms/can/raw_can_msg.h"
#include "shared/util/peripheral.h"

namespace shared::periph {

class CanBase : public util::Peripheral {
public:
    virtual void Send(const shared::can::RawCanMsg&) = 0;
    virtual void ReadQueue(shared::can::RawCanMsg[], size_t len) = 0;
};

}  // namespace shared::periph