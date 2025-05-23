// This should be replaced with FreeRTOS in the future

#pragma once

#include <etl/vector.h>

#include <cstdint>

void scheduler_register_task(void (*task)(void* argument), uint32_t period_ms,
                             void* argument);

void scheduler_run(void);