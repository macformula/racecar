
#pragma once

#include <alerts.hpp>

#include "fc_alert_types.hpp"

namespace alerts {

macfe::Alerts<FcAlert>& GetAlertsManager(void);

}