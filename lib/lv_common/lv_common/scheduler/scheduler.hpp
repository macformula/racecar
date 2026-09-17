// This should be replaced with FreeRTOS in the future

#pragma once

#include <cstdint>

namespace scheduler {

void register_task(void (*task)(void), uint32_t period_ms);

void run(void);

}  // namespace scheduler