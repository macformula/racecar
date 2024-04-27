/// @author Matteo Tullo
/// @date 2024-02-01

#include "cmsis_os2.h"
#include "mcal/stm32f767/os/tick.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../freertos.c
 */
void MX_FREERTOS_Init(void);
}

namespace os {
void InitializeKernel(void) {
    // TODO: Implement error handling
    osKernelInitialize();
    MX_FREERTOS_Init();
}

void StartKernel(void) {
    // TODO: Implement error handling
    osKernelStart();
}

}  // namespace os
