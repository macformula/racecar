#pragma once

#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"

namespace mcal::linux::periph {
class CanBase : public shared::periph::CanBase {
public:
    CanBase(std::string can_iface);

    void Send(const shared::can::RawCanMsg&);
    void ReadQueue(shared::can::RawCanMsg[], size_t len);
};
}  // namespace mcal::linux::periph