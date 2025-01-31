#include "can.hpp"

#include "shared/comms/can/bus.hpp"
#include "shared/comms/can/msg.hpp"

namespace shared::periph {

void CanBase::AddToBus(can::RawMessage msg) {
    bus_->AddMessage(msg, GetTimestamp());
}

void CanBase::RegisterBus(can::Bus* bus) {
    bus_ = bus;
}

}  // namespace shared::periph