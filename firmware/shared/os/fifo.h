/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>
#include "shared/util/os.h"

namespace shared::osDataType {

class Fifo {
public:
    virtual shared::util::osStatus Put(const void * msg_ptr, uint8_t priority) = 0;
    virtual shared::util::osStatus Get(void * msg_buff, uint8_t * prio_buff) = 0;
    virtual uint32_t GetCapacity() = 0;
    virtual uint32_t GetMessageSize() = 0;
    virtual uint32_t GetCount() = 0;
    virtual uint32_t GetSpaceAvailable() = 0;
};

} // namespace shared::os
