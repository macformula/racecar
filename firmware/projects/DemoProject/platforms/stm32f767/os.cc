/// @author Matteo Tullo
/// @date 2024-02-01

#include "os.h"
#include "cmsis_os2.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../freertos.c
 */
void MX_FREERTOS_Init(void);
extern osSemaphoreId_t testSemaphoreHandle;
extern osSemaphoreId_t testBinarySemaphoreHandle;
extern osMutexId_t testMutexHandle;
}

namespace os {
// mcal::os::Semaphore sem_test{testSemaphoreHandle};
mcal::os::Semaphore sem_test{&testBinarySemaphoreHandle};
mcal::os::Mutex mutex_test{&testMutexHandle};

    void InitializeKernel(void) {
        // TODO: Implement error handling
        osKernelInitialize();
        MX_FREERTOS_Init();
    }

    void StartKernel(void) {
        // TODO: Implement error handling
        osKernelStart();
    }

} // namespace os
