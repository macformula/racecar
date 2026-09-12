#include "alerts/alerts.hpp"

namespace alerts {
static LvAlerts state;

LvAlerts& Get() {
    return state;
}

void Reset() {
    state = LvAlerts{};
}
}  // namespace alerts