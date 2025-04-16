/// @author Matteo Tullo
/// @date 2024-02-02

#pragma once

#include "cmsis_os2.h"
#include "shared/os/os.hpp"
#include "shared/os/semaphore.hpp"

namespace mcal::stm32f::os {

// TODO: Add comments and handle errors more robustly
class Semaphore final : public shared::os::Semaphore {
    using OsStatus = shared::os::OsStatus;

public:
    Semaphore(osSemaphoreId_t* sem_id) : sem_id_(sem_id) {}

    OsStatus Acquire() override {
        // TODO: Make this nonblocking
        auto status = osSemaphoreAcquire(*sem_id_, osWaitForever);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }
    OsStatus Release() override {
        auto status = osSemaphoreRelease(*sem_id_);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }
    size_t GetCount() override {
        return osSemaphoreGetCount(*sem_id_);
    }

private:
    osSemaphoreId_t* sem_id_;
};

}  // namespace mcal::stm32f::os
