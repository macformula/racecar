#include "can.hpp"

#include "can/bus.hpp"
#include "can/msg.hpp"

namespace macfe::periph {

void CanBase::AddToBus(can::RawMessage msg) {
    bus_->AddMessage(msg, GetTimestamp());
}

void CanBase::RegisterBus(can::Bus* bus) {
    bus_ = bus;
}

__attribute__((weak)) void CanErrorHandler(CanBase* can_base) {
    (void)can_base;
}

}  // namespace macfe::periph