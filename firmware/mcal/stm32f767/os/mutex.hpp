/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include "cmsis_os2.h"
#include "shared/os/mutex.hpp"
#include "shared/os/os.hpp"

namespace mcal::stm32f767::os {

// TODO: Add comments and handle errors more robustly
class Mutex final : public shared::os::Mutex {
    using OsStatus = shared::os::OsStatus;

public:
    Mutex(osMutexId_t* mutex_id) : mutex_id_(mutex_id) {}

    OsStatus Acquire() override {
        // TODO: Make this nonblocking
        auto status = osMutexAcquire(*mutex_id_, osWaitForever);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }
    OsStatus Release() override {
        auto status = osMutexRelease(*mutex_id_);
        return status == osOK ? OsStatus::kOk : OsStatus::kError;
    }

private:
    osMutexId_t* mutex_id_;
};

}  // namespace mcal::stm32f767::os
