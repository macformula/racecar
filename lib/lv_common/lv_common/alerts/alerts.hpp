#pragma once

namespace alerts {
struct LvAlerts {
    bool hsd_overcurrent = false;
    // Will eventually add more fault signals
};
LvAlerts& Get();
void Reset();
}  // namespace alerts