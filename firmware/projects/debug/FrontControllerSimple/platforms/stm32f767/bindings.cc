/// @author Blake Freer
/// @date 2024-02-27

// cubemx files
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "shared/periph/analog_input.hpp"
#include "shared/periph/can.hpp"
#include "stm32f7xx_hal_adc.h"
#include "tim.h"

// fw imports
#include "bindings.hpp"
#include "mcal/stm32f767/periph/analog_input.hpp"
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

AnalogInput brake_pedal_front{
    &hadc1,
    ADC_CHANNEL_2,  // TODO is front BPPS1 = chn 2?
};
AnalogInput brake_pedal_rear{
    &hadc1,
    ADC_CHANNEL_1,  // TODO is rear BPPS2 = chn 1?
};
DigitalOutput debug_led{
    DEBUG_LED_GPIO_Port,
    DEBUG_LED_Pin,
};
AnalogInput hvil_feedback{
    &hadc1,
    ADC_CHANNEL_5,
};
DigitalOutput dashboard_hsd{
    DASHBOARD_HSD_EN_GPIO_Port,
    DASHBOARD_HSD_EN_Pin,
};
DigitalOutput hvil_led{
    HVIL_LED_EN_GPIO_Port,
    HVIL_LED_EN_Pin,
};
DigitalOutput driver_speaker{
    RTDS_EN_GPIO_Port,
    RTDS_EN_Pin,
};
DigitalInput start_button{
    START_BUTTON_GPIO_Port,
    START_BUTTON_Pin,
};
DigitalOutput brake_light{
    BRAKE_LIGHT_EN_GPIO_Port,
    BRAKE_LIGHT_EN_Pin,
};
DigitalOutput status_light{
    STATUS_LED_EN_GPIO_Port,
    STATUS_LED_EN_Pin,
};
AnalogInput wheel_speed_right_b{
    &hadc1,
    ADC_CHANNEL_10,
};
AnalogInput wheel_speed_right_a{
    &hadc1,
    ADC_CHANNEL_11,
};
AnalogInput accel_pedal_1{
    &hadc1,
    ADC_CHANNEL_12,
};
AnalogInput accel_pedal_2{
    &hadc1,
    ADC_CHANNEL_13,
};
AnalogInput wheel_speed_left_b{
    &hadc1,
    ADC_CHANNEL_14,
};
AnalogInput wheel_speed_left_a{
    &hadc1,
    ADC_CHANNEL_15,
};
AnalogInput steering_wheel{
    &hadc1,
    ADC_CHANNEL_4,
};
AnalogInput suspension_sensor_1{
    &hadc1,
    ADC_CHANNEL_8,
};
AnalogInput suspension_sensor_2{
    &hadc1,
    ADC_CHANNEL_9,
};

CanBase veh_can_base{&hcan3};
CanBase pt_can_base{&hcan1};

}  // namespace mcal
namespace bindings {

// peripherals are ordered as in Pin Mapping spreadsheet

shared::periph::AnalogInput& brake_pedal_front = mcal::brake_pedal_front;
shared::periph::AnalogInput& brake_pedal_rear = mcal::brake_pedal_rear;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;
shared::periph::AnalogInput& hvil_feedback = mcal::hvil_feedback;
shared::periph::DigitalOutput& dashboard_hsd = mcal::dashboard_hsd;
shared::periph::DigitalOutput& hvil_led = mcal::hvil_led;

// (SAM) Confirm that PA11,PA12 is CAN1-powertrain and
// PA8,PA15 is CAN3-vehicle
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::CanBase& pt_can_base = mcal::pt_can_base;

shared::periph::DigitalOutput& driver_speaker = mcal::driver_speaker;
shared::periph::DigitalInput& start_button = mcal::start_button;
shared::periph::DigitalOutput& brake_light = mcal::brake_light;
shared::periph::DigitalOutput& status_light = mcal::status_light;
shared::periph::AnalogInput& wheel_speed_right_b = mcal::wheel_speed_right_b;
shared::periph::AnalogInput& wheel_speed_right_a = mcal::wheel_speed_right_a;
shared::periph::AnalogInput& accel_pedal_1 = mcal::accel_pedal_1;
shared::periph::AnalogInput& accel_pedal_2 = mcal::accel_pedal_2;
shared::periph::AnalogInput& wheel_speed_left_b = mcal::wheel_speed_left_b;
shared::periph::AnalogInput& wheel_speed_left_a = mcal::wheel_speed_left_a;
shared::periph::AnalogInput& steering_wheel = mcal::steering_wheel;
shared::periph::AnalogInput& suspension_sensor_1 = mcal::suspension_sensor_1;
shared::periph::AnalogInput& suspension_sensor_2 = mcal::suspension_sensor_2;

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_TIM2_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN3_Init();

    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
}

}  // namespace bindings
