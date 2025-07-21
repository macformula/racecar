#include <cstdint>

#include "Components/nt35510/nt35510.h"
#include "Components/otm8009a/otm8009a.h"
#include "stm32f4xx_hal.h"

void NT35510_IO_Delay(uint32_t Delay) {
    HAL_Delay(Delay);
}

void OTM8009A_IO_Delay(uint32_t Delay) {
    HAL_Delay(Delay);
}