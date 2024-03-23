/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>
#include "shared/util/os.h"

namespace shared::osDataType {

class EventFlags {
public:
    virtual shared::util::OsStatus Acquire() = 0;
    virtual shared::util::OsStatus Release() = 0;
};

} // namespace shared::os
