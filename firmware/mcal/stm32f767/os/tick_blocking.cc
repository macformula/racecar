/// @author Samuel Parent
/// @date 2024-04-15

#include <cstdint>

#include "stm32f7xx_hal.h"

/// @note This file should be left seperate from tick.h
///       as it allows for timing control in projects that
///       do not make use of an RTOS.

namespace os {
void TickBlocking(uint32_t ticks) {
    HAL_Delay(ticks);
}
}  // namespace os
