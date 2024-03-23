/// @author Matteo Tullo
/// @date 2024-02-24

#pragma once

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/mutex.h"
#include "cmsis_os2.h"

namespace mcal::os {

// TODO: Add comments and handle errors more robustly
class Mutex final : public shared::os::Mutex  {
private:
    osMutexId_t* mutex_id;

public:
    Mutex(osMutexId_t* mutex_id_)
        : mutex_id(mutex_id_) {}

    shared::util::OsStatus Acquire() override {
        // TODO: Make this nonblocking
        osMutexAcquire(*mutex_id, osWaitForever);
        return shared::util::OsStatus::kOsOk;
    }
    shared::util::OsStatus Release() override {
        osMutexRelease(*mutex_id);
        return shared::util::OsStatus::kOsOk;
    }
};

}  // namespace mcal::os
