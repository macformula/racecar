/// @author Blake Freer
/// @date 2024-04-02

#include "cmsis_os2.h"
#include "mcal/stm32f767/os/tick.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../freertos.c
 */
void MX_FREERTOS_Init();
}

namespace os {
void InitializeKernel() {
    osKernelInitialize();
    MX_FREERTOS_Init();
}

void StartKernel() {
    osKernelStart();
}

}  // namespace os