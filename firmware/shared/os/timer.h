/// @author Matteo Tullo
/// @date 2024-03-09

#pragma once

#include <cstdint>

#include "shared/os/os.h"

namespace shared::os {

class Timer {
public:
    virtual OsStatus Start(uint32_t ticks) = 0;
    virtual OsStatus Stop() = 0;
    virtual bool IsRunning() = 0;
};

}  // namespace shared::os
