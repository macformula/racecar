#include <cstdint>

#include "adc.h"
#include "bindings.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "shared/periph/adc.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_adc.h"

// fw imports
#include "mcal/stm32f767/periph/adc.hpp"
#include "mcal/stm32f767/periph/can.hpp"
#include "mcal/stm32f767/periph/gpio.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

ADCInput accel_pedal_1{
    &hadc1,
    ADC_CHANNEL_12,
};
ADCInput accel_pedal_2{
    &hadc1,
    ADC_CHANNEL_13,
};

CanBase vehicle_can_base{&hcan3};

DigitalOutput debug_led{
    DEBUG_LED_GPIO_Port,
    DEBUG_LED_Pin,
};
}  // namespace mcal

namespace bindings {
shared::periph::ADCInput& accel_pedal_1 = mcal::accel_pedal_1;
shared::periph::ADCInput& accel_pedal_2 = mcal::accel_pedal_2;
shared::periph::CanBase& vehicle_can_base = mcal::vehicle_can_base;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_ADC1_Init();
    MX_GPIO_Init();
    MX_CAN3_Init();

    mcal::vehicle_can_base.Setup();
}

void DelayMS(uint32_t ms) {
    HAL_Delay(ms);
}

}  // namespace bindings