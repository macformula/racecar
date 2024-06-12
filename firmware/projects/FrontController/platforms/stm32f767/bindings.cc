/// @author Blake Freer
/// @date 2024-02-27

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "stm32f7xx_hal_adc.h"
#include "tim.h"

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

// peripherals are ordered as in Pin Mapping spreadsheet

shared::periph::ADCInput&& brake_pedal_front = ADCInput{
    &hadc1,
    ADC_CHANNEL_2,  // TODO is front BPPS1 = chn 2?
};
shared::periph::ADCInput&& brake_pedal_rear = ADCInput{
    &hadc1,
    ADC_CHANNEL_1,  // TODO is rear BPPS2 = chn 1?
};

shared::periph::DigitalOutput&& debug_led = DigitalOutput{
    DEBUG_LED_GPIO_Port,
    DEBUG_LED_Pin,
};

shared::periph::ADCInput&& hvil_feedback = ADCInput{
    &hadc1,
    ADC_CHANNEL_5,
};

shared::periph::DigitalOutput&& dashboard_hsd = DigitalOutput{
    DASHBOARD_HSD_EN_GPIO_Port,
    DASHBOARD_HSD_EN_Pin,
};

shared::periph::DigitalOutput&& hvil_led = DigitalOutput{
    HVIL_LED_EN_GPIO_Port,
    HVIL_LED_EN_Pin,
};

// (SAM) Confirm that PA11,PA12 is CAN1-powertrain and PA8,PA15 is CAN3-vehicle
shared::periph::CanBase&& veh_can_base = CanBase{&hcan3};
shared::periph::CanBase&& pt_can_base = CanBase{&hcan1};

shared::periph::DigitalOutput&& driver_speaker = DigitalOutput{
    RTDS_EN_GPIO_Port,
    RTDS_EN_Pin,
};

shared::periph::DigitalInput&& start_button = DigitalInput{
    START_BUTTON_GPIO_Port,
    START_BUTTON_Pin,
};

shared::periph::DigitalOutput&& brake_light = DigitalOutput{
    BRAKE_LIGHT_EN_GPIO_Port,
    BRAKE_LIGHT_EN_Pin,
};

shared::periph::DigitalOutput&& status_light = DigitalOutput{
    STATUS_LED_EN_GPIO_Port,
    STATUS_LED_EN_Pin,
};

shared::periph::ADCInput&& wheel_speed_right_b = ADCInput{
    &hadc1,
    ADC_CHANNEL_10,
};
shared::periph::ADCInput&& wheel_speed_right_a = ADCInput{
    &hadc1,
    ADC_CHANNEL_11,
};

shared::periph::ADCInput&& accel_pedal_1 = ADCInput{
    &hadc1,
    ADC_CHANNEL_12,
};
shared::periph::ADCInput&& accel_pedal_2 = ADCInput{
    &hadc1,
    ADC_CHANNEL_13,
};

shared::periph::ADCInput&& wheel_speed_left_b = ADCInput{
    &hadc1,
    ADC_CHANNEL_14,
};
shared::periph::ADCInput&& wheel_speed_left_a = ADCInput{
    &hadc1,
    ADC_CHANNEL_15,
};

shared::periph::ADCInput&& steering_wheel = ADCInput{
    &hadc1,
    ADC_CHANNEL_4,
};

shared::periph::ADCInput&& suspension_sensor_1 = ADCInput{
    &hadc1,
    ADC_CHANNEL_8,
};
shared::periph::ADCInput&& suspension_sensor_2 = ADCInput{
    &hadc1,
    ADC_CHANNEL_9,
};

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_TIM2_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN3_Init();
}

}  // namespace bindings
