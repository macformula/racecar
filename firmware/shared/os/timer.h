/// @author Matteo Tullo
/// @date 2024-03-09

#pragma once

#include <cstdint>
#include "shared/util/os.h"

namespace shared::osDataType {

class Timer {
public:
    virtual shared::util::osStatus Start(uint32_t ticks) = 0;
    virtual shared::util::osStatus Stop() = 0;
    virtual uint32_t IsRunning() = 0;
};

} // namespace shared::os
