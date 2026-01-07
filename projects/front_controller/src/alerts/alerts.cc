#include "alerts.hpp"

// Include the template implementation
#include <alerts.cc>

namespace alerts {

macfe::Alerts<FcAlert>& GetAlertsManager(void) {
    static macfe::Alerts<FcAlert> instance;
    return instance;
}

}  // namespace alerts

// forces compilation of all methods
template class macfe::Alerts<alerts::FcAlert>;

void CanErrorHandler(void) {
    alerts::GetAlertsManager().Set(alerts::FcAlert::CanTxError);
}