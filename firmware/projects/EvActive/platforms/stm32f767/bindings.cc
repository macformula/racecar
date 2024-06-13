/// @author Blake Freer
/// @date 2024-02-27

// cubemx files
#include "adc.h"
#include "can.h"
#include "cubemx/Inc/main.h"
#include "gpio.h"

// fw imports
#include "mcal/stm32f767/periph/adc.h"
#include "mcal/stm32f767/periph/can.h"
#include "mcal/stm32f767/periph/gpio.h"
#include "shared/periph/can.h"

namespace mcal {
using namespace stm32f767::periph;

CanBase veh_can_base{&hcan3};

ADCInput accel_pedal_pos_1{&hadc1, ADC_CHANNEL_12};
ADCInput accel_pedal_pos_2{&hadc1, ADC_CHANNEL_13};
ADCInput brake_pedal_pos_1{&hadc1, ADC_CHANNEL_2};
ADCInput brake_pedal_pos_2{&hadc1, ADC_CHANNEL_1};
ADCInput suspension_travel_1{&hadc1, ADC_CHANNEL_8};
ADCInput suspension_travel_2{&hadc1, ADC_CHANNEL_9};
ADCInput hvil_feedback{&hadc1, ADC_CHANNEL_5};
ADCInput steering_angle{&hadc1, ADC_CHANNEL_4};

DigitalInput start_button_n{START_BUTTON_N_GPIO_Port, START_BUTTON_N_Pin};
DigitalInput wheel_speed_left_a{WHEEL_SPEED_LEFT_A_GPIO_Port,
                                WHEEL_SPEED_LEFT_A_Pin};
DigitalInput wheel_speed_left_b{WHEEL_SPEED_LEFT_B_GPIO_Port,
                                WHEEL_SPEED_LEFT_B_Pin};
DigitalInput wheel_speed_right_a{WHEEL_SPEED_RIGHT_A_GPIO_Port,
                                 WHEEL_SPEED_RIGHT_A_Pin};
DigitalInput wheel_speed_right_b{WHEEL_SPEED_RIGHT_A_GPIO_Port,
                                 WHEEL_SPEED_RIGHT_A_Pin};

DigitalOutput debug_led_en{DEBUG_LED_GPIO_Port, DEBUG_LED_Pin};
DigitalOutput dashboard_en{DASHBOARD_HSD_EN_GPIO_Port, DASHBOARD_HSD_EN_Pin};
DigitalOutput hvil_led_en{HVIL_LED_EN_GPIO_Port, HVIL_LED_EN_Pin};
DigitalOutput brake_light_en{BRAKE_LIGHT_EN_GPIO_Port, BRAKE_LIGHT_EN_Pin};
DigitalOutput status_led_en{STATUS_LED_EN_GPIO_Port, STATUS_LED_EN_Pin};
DigitalOutput rtds_en{RTDS_EN_GPIO_Port, RTDS_EN_Pin};

}  // namespace mcal

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;

shared::periph::ADCInput& accel_pedal_pos_1 = mcal::accel_pedal_pos_1;
shared::periph::ADCInput& accel_pedal_pos_2 = mcal::accel_pedal_pos_2;
shared::periph::ADCInput& brake_pedal_pos_1 = mcal::brake_pedal_pos_1;
shared::periph::ADCInput& brake_pedal_pos_2 = mcal::brake_pedal_pos_2;
shared::periph::ADCInput& suspension_travel_1 = mcal::suspension_travel_1;
shared::periph::ADCInput& suspension_travel_2 = mcal::suspension_travel_2;
shared::periph::ADCInput& hvil_feedback = mcal::hvil_feedback;
shared::periph::ADCInput& steering_angle = mcal::steering_angle;

shared::periph::DigitalInput& start_button_n = mcal::start_button_n;
shared::periph::DigitalInput& wheel_speed_left_a = mcal::wheel_speed_left_a;
shared::periph::DigitalInput& wheel_speed_left_b = mcal::wheel_speed_left_b;
shared::periph::DigitalInput& wheel_speed_right_a = mcal::wheel_speed_right_a;
shared::periph::DigitalInput& wheel_speed_right_b = mcal::wheel_speed_right_b;

shared::periph::DigitalOutput& debug_led_en = mcal::debug_led_en;
shared::periph::DigitalOutput& dashboard_en = mcal::dashboard_en;
shared::periph::DigitalOutput& hvil_led_en = mcal::hvil_led_en;
shared::periph::DigitalOutput& brake_light_en = mcal::brake_light_en;
shared::periph::DigitalOutput& status_led_en = mcal::status_led_en;
shared::periph::DigitalOutput& rtds_en = mcal::rtds_en;

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN3_Init();
    mcal::veh_can_base.Setup();
}

void TickBlocking(uint32_t ticks) {
    HAL_Delay(ticks);
}

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan3) {
        mcal::veh_can_base.AddRxMessageToQueue();
    }
}
}

}  // namespace bindings
