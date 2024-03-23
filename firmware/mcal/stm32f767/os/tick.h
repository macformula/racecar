/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "cmsis_os2.h"

// TODO: Add comments and handle errors more robustly
namespace os {
void Tick(uint32_t ticks) {
    osDelay(ticks);
}

void TickUntil(uint32_t ticks) {
    osDelayUntil(ticks);
}

uint32_t GetTickCount() {
    return osKernelGetTickCount();
}

}  // namespace mcal::os

