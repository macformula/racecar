
#include "adc.h"
#include "can.h"
#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "bindings.h"
#include "stm32f7xx_hal_adc.h"

// fw imports
#include "mcal/stm32f767/periph/adc.h"
#include "mcal/stm32f767/periph/can.h"
#include "mcal/stm32f767/periph/gpio.h"

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

CanBase veh_can_base{&hcan3};

}

namespace bindings {
shared::periph::ADCInput& accel_pedal_1 = mcal::accel_pedal_1;
shared::periph::ADCInput& accel_pedal_2 = mcal::accel_pedal_2;

void Initialize() {
    HAL_Init();

    MX_ADC1_Init();
    MX_CAN3_Init();

    mcal::veh_can_base.Setup();
}

}  // namespace bindings