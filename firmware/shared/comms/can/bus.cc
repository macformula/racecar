#include "bus.h"
#include "shared/periph/can.h"

namespace shared::can {

Bus::Bus(periph::CanBase& can_base) : can_base_(can_base) {
    can_base_.RegisterBus(this);
}

}  // namespace shared::can