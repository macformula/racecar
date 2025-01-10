#include "../../bindings.hpp"

#include "adc.h"
#include "main.h"
#include "mcal/stm32f767/periph/analog_input.hpp"
#include "shared/periph/analog_input.hpp"
#include "stm32f7xx_hal.h"

namespace mcal {

stm32f767::periph::AnalogInput analog_input{&hadc1, ADC_CHANNEL_11};

}  // namespace mcal

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {

shared::periph::AnalogInput& analog_input = mcal::analog_input;

void Init() {
    SystemClock_Config();
    HAL_Init();
    MX_ADC1_Init();
}

}  // namespace bindings