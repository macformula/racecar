/// @author Samuel Parent
/// @date 2024-04-15

#include <chrono>
#include <cstdint>
#include <ratio>
#include <thread>

namespace os {
void TickBlocking(uint32_t ticks) {
    std::chrono::milliseconds duration(ticks);

    std::this_thread::sleep_for(duration);
}
}  // namespace os
