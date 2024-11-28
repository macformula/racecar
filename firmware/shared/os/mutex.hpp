/// @author Matteo Tullo
/// @date 2024-02-02

#pragma once

#include "shared/os/os.hpp"

namespace shared::os {

class Mutex {
public:
    virtual OsStatus Acquire() = 0;
    virtual OsStatus Release() = 0;
};

}  // namespace shared::os
