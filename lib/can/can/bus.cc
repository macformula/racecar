#include "bus.hpp"

#include "periph/can.hpp"

namespace macfe::can {

Bus::Bus(periph::CanBase& can_base) : can_base_(can_base) {
    can_base_.RegisterBus(this);
}

}  // namespace macfe::can
