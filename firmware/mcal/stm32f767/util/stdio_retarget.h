#pragma once

#include <stdio.h>
#include <sys/stat.h>

#include "stm32f7xx_hal.h"

#ifdef HAL_UART_MODULE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

void SetDebugUART(UART_HandleTypeDef* huart);

int _write(int fd, char* ptr, int len);
int _read(int fd, char* ptr, int len);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // HAL_UART_MODULE_ENABLED
