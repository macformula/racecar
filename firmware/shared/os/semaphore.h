/// @author Matteo Tullo
/// @date 2024-02-02

#pragma once

#include <cstdint>
#include "shared/util/os.h"

namespace shared::osDataType {

class Semaphore {
public:
    virtual shared::util::osStatus Acquire() = 0;
    virtual shared::util::osStatus Release() = 0;
    virtual uint32_t GetCount() = 0;
};

} // namespace shared::os
