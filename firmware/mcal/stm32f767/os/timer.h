/// @author Matteo Tullo
/// @date 2024-03-09

#pragma once

#include <cstdint>

#include "cmsis_os2.h"
#include "shared/os/os.h"
#include "shared/os/timer.h"

namespace mcal::os {

// TODO: Add comments and handle errors more robustly
class Timer final : public shared::os::Timer {
    using OsStatus = shared::os::OsStatus;

public:
    Timer(osTimerId_t* timer_id) : timer_id_(timer_id) {}

    OsStatus Start(uint32_t ticks) override {
        auto ret = osTimerStart(*timer_id_, ticks);
        return ret == osOK ? OsStatus::kOk : OsStatus::kError;
    }
    OsStatus Stop() override {
        auto ret = osTimerStop(*timer_id_);
        return ret == osOK ? OsStatus::kOk : OsStatus::kError;
    }
    bool IsRunning() override {
        return osTimerIsRunning(*timer_id_);
    }

private:
    osTimerId_t* timer_id_;
};

}  // namespace mcal::os
