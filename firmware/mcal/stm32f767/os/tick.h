/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/tick.h"
#include "cmsis_os2.h"

namespace shared::os {
void Tick(uint32_t ticks) {
    osDelay(ticks);
}

void TickUntil(uint32_t ticks) {
    osDelayUntil(ticks);
}

uint32_t GetTickCount() {
    return osKernelGetTickCount();
}

}

