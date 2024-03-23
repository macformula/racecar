/// @author Matteo Tullo
/// @date 2024-02-01

#include "cmsis_os2.h"

#include "mcal/stm32f767/os/semaphore.h"
#include "mcal/stm32f767/os/mutex.h"
#include "mcal/stm32f767/os/fifo.h"
#include "mcal/stm32f767/os/timer.h"
#include "mcal/stm32f767/os/tick.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../freertos.c
 */
void MX_FREERTOS_Init(void);
extern osSemaphoreId_t testSemaphoreHandle;
extern osSemaphoreId_t testBinarySemaphoreHandle;
extern osMutexId_t testMutexHandle;
extern osMessageQueueId_t myTestQueueHandle;
extern osTimerId_t messageTimerHandle;
}

namespace mcal {
os::Semaphore sem_test{&testBinarySemaphoreHandle};
os::Mutex mutex_test{&testMutexHandle};
os::Fifo message_queue_test{&myTestQueueHandle};
os::Timer timer_test{&messageTimerHandle};
}   // namespace mcal

namespace os {
const shared::os::Semaphore& sem_test = mcal::sem_test;
const shared::os::Mutex& mutex_test = mcal::mutex_test;
const shared::os::Fifo& message_queue_test = mcal::message_queue_test;
const shared::os::Timer& timer_test = mcal::timer_test;

void InitializeKernel(void) {
    // TODO: Implement error handling
    osKernelInitialize();
    MX_FREERTOS_Init();
}

void StartKernel(void) {
    // TODO: Implement error handling
    osKernelStart();
}

}   // namespace os
