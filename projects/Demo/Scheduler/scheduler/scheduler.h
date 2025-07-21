// This should be replaced with FreeRTOS in the future

#pragma once

#include <etl/vector.h>

#include <cstdint>

void scheduler_register_task(void (*task)(void), uint32_t period_ms);

void scheduler_run(void);