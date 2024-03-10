/// @author Matteo Tullo
/// @date 2024-02-02

#ifndef MCAL_STM32F767_OS_SEMAPHORE_H_
#define MCAL_STM32F767_OS_SEMAPHORE_H_

#include <cstdint>

#include "shared/util/os.h"
#include "shared/os/semaphore.h"
#include "cmsis_os2.h"

namespace mcal::os {

// TODO: Add comments and handle errors more robustly
class Semaphore final : public shared::osDataType::Semaphore  {
private:
    osSemaphoreId_t* sem_id;

public:
    Semaphore(osSemaphoreId_t* sem_id_)
        : sem_id(sem_id_) {}

    shared::util::osStatus Acquire() override {
        // TODO: Make this nonblocking
        osSemaphoreAcquire(*sem_id, osWaitForever);
        return shared::util::osStatus::osOk;
    }
    shared::util::osStatus Release() override {
        osSemaphoreRelease(*sem_id);
        return shared::util::osStatus::osOk;
    }
    uint32_t GetCount() override {
        return osSemaphoreGetCount(*sem_id);
    }
};

}  // namespace mcal::os

#endif