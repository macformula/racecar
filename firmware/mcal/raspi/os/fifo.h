/// @author Blake Freer
/// @date 2024-04-17

#pragma once

#include "shared/os/fifo.h"
#include "shared/os/os.h"

namespace mcal::raspi {

class Fifo : public shared::os::Fifo {
    using OsStatus = shared::os::OsStatus;

public:
    Fifo() {}

    OsStatus Put(const void* msg_ptr, uint8_t priority) override {}

    OsStatus Get(void* msg_buff, uint8_t* prio_buff) override {}

    size_t GetCapacity() override {}

    size_t GetMessageSize() override {}
    size_t GetCount() override {}
    size_t GetSpaceAvailable() override {}

private:
};

}  // namespace mcal::raspi