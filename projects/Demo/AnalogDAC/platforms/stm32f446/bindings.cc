/// @author Tamara Xu
/// @date 2025-01-25

// cubemx files
#include "dac.h"
#include "stm32f4xx_hal.h"

// fw includes
#include "../../bindings.hpp"
#include "mcal/stm32f/analog_output_dac.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "shared/periph/analog_output.hpp"
#include "shared/periph/gpio.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}
namespace mcal {
using namespace stm32f;

AnalogOutputDAC analog_out_dac{&hdac, DAC_CHANNEL_1};  // using PA4

}  // namespace mcal

namespace bindings {

shared::periph::AnalogOutput& analog_out_dac = mcal::analog_out_dac;

void DelayMS(unsigned int ms) {
    HAL_Delay(ms);
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_DAC_Init();
}

}  // namespace bindings
