/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/fifo.h"
#include "cmsis_os2.h"

namespace mcal::os {

class Fifo final : public shared::osDataType::Fifo  {
private:
    osMessageQueueId_t* fifo_id;

public:
    Fifo(osMessageQueueId_t* fifo_id_)
        : mutex_id(fifo_id_) {}
    
    shared::util::osStatus Put(const void * msg_ptr, uint8_t priority) override {
        osMessageQueuePut(*fifo_id, msg_ptr, priority, 0u);
        return shared::util::osStatus::osOk;
    }

    shared::util::osStatus Get(const void * msg_buff, uint8_t * prio_buff) override {
        int ret = osMessageQueueGet(*fifo_id, msg_buff, prio_buff, 0u);
        return ret == 0 ? shared::util::osStatus::osOk : shared::util::osStatus::osError;
    }

    uint32_t GetCapacity() override {
        return osMessageQueueGetCapacity(*fifo_id);
    }

    uint32_t GetMessageSize() override {
        return osMessageQueueGetMsgSize(*fifo_id);
    }
    
    uint32_t GetCount() override {
        return osMessageQueueGetCount(*fifo_id);
    }

    uint32_t GetSpaceAvailable() override {
        return osMessageQueueGetSpace(*fifo_id);
    }
};

}  // namespace mcal::os
