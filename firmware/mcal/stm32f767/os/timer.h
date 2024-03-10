/// @author Matteo Tullo
/// @date 2024-03-09

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/timer.h"
#include "cmsis_os2.h"

namespace mcal::os {


// TODO: Add comments and handle errors more robustly
class Timer final : public shared::osDataType::Timer  {
private:
    osTimerId_t* timer_id;

public:
    Timer(osTimerId_t* timer_id_)
        : timer_id(timer_id_) {}
    
    shared::util::osStatus Start(uint32_t ticks) override {
        int ret = osTimerStart(*timer_id, ticks);
        return ret == 0 ? shared::util::osStatus::osOk : shared::util::osStatus::osError;
    }
    shared::util::osStatus Stop() override {
        int ret = osTimerStop(*timer_id);
        return ret == 0 ? shared::util::osStatus::osOk : shared::util::osStatus::osError;
    }
    uint32_t IsRunning() override {
        return osTimerIsRunning(*timer_id);
    }
};

}  // namespace mcal::os
