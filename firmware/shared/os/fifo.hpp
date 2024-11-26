/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstddef>
#include <cstdint>

#include "shared/os/os.hpp"

namespace shared::os {

class Fifo {
public:
    virtual OsStatus Put(const void* msg_ptr, uint8_t priority) = 0;
    virtual OsStatus Get(void* msg_buff, uint8_t* prio_buff) = 0;
    virtual size_t GetCapacity() = 0;
    virtual size_t GetMessageSize() = 0;
    virtual size_t GetCount() = 0;
    virtual size_t GetSpaceAvailable() = 0;
};

}  // namespace shared::os
