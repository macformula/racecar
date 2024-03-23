/// @author Matteo Tullo
/// @date 2024-02-02

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/semaphore.h"
#include "cmsis_os2.h"

namespace mcal::os {

// TODO: Add comments and handle errors more robustly
class Semaphore final : public shared::os::Semaphore  {
private:
    osSemaphoreId_t* sem_id;

public:
    Semaphore(osSemaphoreId_t* sem_id_)
        : sem_id(sem_id_) {}

    shared::util::OsStatus Acquire() override {
        // TODO: Make this nonblocking
        osSemaphoreAcquire(*sem_id, osWaitForever);
        return shared::util::OsStatus::kOsOk;
    }
    shared::util::OsStatus Release() override {
        osSemaphoreRelease(*sem_id);
        return shared::util::OsStatus::kOsOk;
    }
    uint32_t GetCount() override {
        return osSemaphoreGetCount(*sem_id);
    }
};

}  // namespace mcal::os
