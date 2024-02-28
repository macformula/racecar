/// @author Blake Freer
/// @date 2024-02-27

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

// fw imports
#include "mcal/stm32f767/periph/adc.h"
#include "mcal/stm32f767/periph/gpio.h"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {

mcal::periph::DigitalOutput driver_speaker{RTDS_EN_GPIO_Port, RTDS_EN_Pin};
mcal::periph::DigitalOutput brake_light{BRAKE_LIGHT_EN_GPIO_Port,
                                        BRAKE_LIGHT_EN_Pin};
mcal::periph::ADCInput accel_pedal_1{&hadc1, ADC_CHANNEL_10};
mcal::periph::ADCInput accel_pedal_2{&hadc1, ADC_CHANNEL_11};
mcal::periph::ADCInput steering_wheel{&hadc1, ADC_CHANNEL_12};
mcal::periph::ADCInput brake_pedal{&hadc1, ADC_CHANNEL_13};
mcal::periph::DigitalInput driver_button{START_BUTTON_N_GPIO_Port,
                                         START_BUTTON_N_Pin};

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
