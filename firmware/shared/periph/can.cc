#include "can.h"

#include "shared/comms/can/bus.h"
#include "shared/comms/can/msg.h"

namespace shared::periph {

void CanBase::Receive(can::RawMessage msg) {
    bus_->AddMessage(msg, GetTimestamp());
}

void CanBase::RegisterBus(can::Bus* bus) {
    bus_ = bus;
}

}  // namespace shared::periph