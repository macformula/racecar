#include "alerts.hpp"

#include "generated/can/veh_messages.hpp"

namespace alerts {

using namespace generated::can;

static TxFcAlerts msg;

TxFcAlerts& Get(void) {
    return msg;
}

void Reset(void) {
    msg = TxFcAlerts{};
}

}  // namespace alerts

// from shared/perip/can
void CanErrorHandler(void) {
    alerts::Get().can_tx_error = true;
}
