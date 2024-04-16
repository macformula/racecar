/// @author Blake Freer
/// @date 2024-03-24
/// @note These functions must be implemented in each mcal

namespace os {

void Tick(uint32_t ticks);
void TickUntil(uint32_t ticks);
void TickBlocking(uint32_t ticks);
uint32_t GetTickCount();

}  // namespace os
