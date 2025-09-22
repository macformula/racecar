#include "bindings.hpp"

#include <unistd.h>

#include <chrono>
#include <iostream>

namespace bindings {

uint32_t GetTickMs() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
        .count();
}

}  // namespace bindings