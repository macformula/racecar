/// @author Matteo Tullo
/// @date 2024-02-16

#pragma once

#include <cstdint>

namespace shared::os {
void Tick(uint32_t ticks);
void TickUntil(uint32_t ticks);
uint32_t GetTickCount();

}   // namespace shared::os