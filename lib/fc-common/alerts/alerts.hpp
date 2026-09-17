#pragma once

#include "generated/can/veh_messages.hpp"

namespace alerts {

generated::can::TxFcAlerts& Get(void);

void Reset(void);

}  // namespace alerts