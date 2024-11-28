/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include "cmsis_os2.h"
#include "shared/os/fifo.hpp"
#include "shared/os/os.hpp"

namespace mcal::stm32f767::os {

// TODO: Add comments and handle errors more robustly
class Fifo final : public shared::os::Fifo {
    using OsStatus = shared::os::OsStatus;

public:
    Fifo(osMessageQueueId_t* fifo_id) : fifo_id_(fifo_id) {}

    shared::os::OsStatus Put(const void* msg_ptr, uint8_t priority) override {
        auto status = osMessageQueuePut(*fifo_id_, msg_ptr, priority, 0u);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }

    OsStatus Get(void* msg_buff, uint8_t* prio_buff) override {
        auto status = osMessageQueueGet(*fifo_id_, msg_buff, prio_buff, 0u);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }

    size_t GetCapacity() override {
        return osMessageQueueGetCapacity(*fifo_id_);
    }

    size_t GetMessageSize() override {
        return osMessageQueueGetMsgSize(*fifo_id_);
    }

    size_t GetCount() override {
        return osMessageQueueGetCount(*fifo_id_);
    }

    size_t GetSpaceAvailable() override {
        return osMessageQueueGetSpace(*fifo_id_);
    }

private:
    osMessageQueueId_t* fifo_id_;
};

}  // namespace mcal::stm32f767::os
