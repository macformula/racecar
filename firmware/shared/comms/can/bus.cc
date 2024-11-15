#include "bus.h"
#include "shared/periph/can.h"

namespace shared::can {

Bus::Bus(periph::CanBase& can_base) : can_base_(can_base) {
    can_base_.RegisterBus(this);
}

void Bus::Send(RawMessage msg) {
    can_base_.Send(msg);
}

}  // namespace shared::can