#include "bus.hpp"
#include "shared/periph/can.hpp"

namespace shared::can {

Bus::Bus(periph::CanBase& can_base) : can_base_(can_base) {
    can_base_.RegisterBus(this);
}

}  // namespace shared::can