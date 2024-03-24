/// @author Matteo Tullo
/// @date 2024-02-02

#pragma once

#include <cstddef>

#include "shared/os/os.h"

namespace shared::os {

class Semaphore {
public:
    virtual OsStatus Acquire() = 0;
    virtual OsStatus Release() = 0;
    virtual size_t GetCount() = 0;
};

}  // namespace shared::os
