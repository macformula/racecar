#include "../../bindings.hpp"

#include "dac.h"
#include "mcal/stm32f/periph/analog_output_dac.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f::periph;

AnalogOutputDAC analog_out_dac{&hdac, DAC_CHANNEL_1};

}  // namespace mcal

namespace bindings {
shared::periph::AnalogOutput& analog_out_dac = mcal::analog_out_dac;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_DAC_Init();
}

void DelayMS(unsigned int ms) {
    HAL_Delay(ms);
}

}  // namespace bindings
