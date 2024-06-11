/// @author Blake Freer
/// @date 2024-02-27

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "shared/periph/can.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

// fw imports
#include "bindings.h"
#include "mcal/stm32f767/periph/adc.h"
#include "mcal/stm32f767/periph/can.h"
#include "mcal/stm32f767/periph/gpio.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {
using namespace mcal::stm32f767::periph;

shared::periph::DigitalOutput&& driver_speaker = DigitalOutput{
    RTDS_EN_GPIO_Port,
    RTDS_EN_Pin,
};
shared::periph::DigitalOutput&& brake_light = DigitalOutput{
    BRAKE_LIGHT_EN_GPIO_Port,
    BRAKE_LIGHT_EN_Pin,
};
shared::periph::DigitalOutput&& status_light = DigitalOutput{
    STATUS_LED_GPIO_Port,
    STATUS_LED_Pin,
};
shared::periph::ADCInput&& accel_pedal_1 = ADCInput{
    &hadc1,
    ADC_CHANNEL_10,
};
shared::periph::ADCInput&& accel_pedal_2 = ADCInput{
    &hadc1,
    ADC_CHANNEL_11,
};
shared::periph::ADCInput&& steering_wheel = ADCInput{
    &hadc1,
    ADC_CHANNEL_12,
};
shared::periph::ADCInput&& front_brake_pedal = ADCInput{
    &hadc1,
    ADC_CHANNEL_13,
};
shared::periph::DigitalInput&& start_button = DigitalInput{
    START_BUTTON_N_GPIO_Port,
    START_BUTTON_N_Pin,
};

// (SAM) idk if hcan1 is correct, change if necessary
shared::periph::CanBase&& motor_left_can = CanBase{&hcan1};
shared::periph::CanBase&& motor_right_can = CanBase{&hcan1};
shared::periph::CanBase&& contactor_can = CanBase{&hcan1};

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_TIM2_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();
    MX_CAN3_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_SPI4_Init();
}

}  // namespace bindings
