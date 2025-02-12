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

// fw imports
#include "../../bindings.hpp"
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

// =========== CAN =========================================
CanBase veh_can_base{&hcan3};
CanBase pt_can_base{&hcan1};

// =========== Vehicle Dynamics Sensors ====================
AnalogInput suspension_travel1{&hadc1, ADC_CHANNEL_8};
AnalogInput suspension_travel2{&hadc1, ADC_CHANNEL_9};
AnalogInput wheel_speed_front_left{&hadc1, ADC_CHANNEL_15};   // A
AnalogInput wheel_speed_front_right{&hadc1, ADC_CHANNEL_11};  // A
AnalogInput wheel_speed_rear_left{&hadc1, ADC_CHANNEL_14};    // B
AnalogInput wheel_speed_rear_right{&hadc1, ADC_CHANNEL_10};   // B

// =========== Driver Control ==============================
DigitalInput start_button{START_BUTTON_GPIO_Port, START_BUTTON_Pin};

AnalogInput steering_angle_sensor{&hadc1, ADC_CHANNEL_4};
AnalogInput brake_pressure_sensor1{&hadc1, ADC_CHANNEL_2};
AnalogInput brake_pressure_sensor2{&hadc1, ADC_CHANNEL_1};
AnalogInput accel_pedal_sensor1{&hadc1, ADC_CHANNEL_12};
AnalogInput accel_pedal_sensor2{&hadc1, ADC_CHANNEL_13};

// =========== Outputs =====================================
DigitalOutput dashboard_power_en{DASHBOARD_HSD_EN_GPIO_Port,
                                 DASHBOARD_HSD_EN_Pin};
DigitalOutput hvil_led_en{HVIL_LED_EN_GPIO_Port, HVIL_LED_EN_Pin};
DigitalOutput rtds_en{RTDS_EN_GPIO_Port, RTDS_EN_Pin};
DigitalOutput brake_light_en{BRAKE_LIGHT_EN_GPIO_Port, BRAKE_LIGHT_EN_Pin};
DigitalOutput status_led_en{STATUS_LED_EN_GPIO_Port, STATUS_LED_EN_Pin};
DigitalOutput debug_led{DEBUG_LED_GPIO_Port, DEBUG_LED_Pin};

}  // namespace mcal
namespace bindings {

// =========== CAN =========================================
shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::CanBase& pt_can_base = mcal::pt_can_base;

// =========== Vehicle Dynamics Sensors ====================
shared::periph::AnalogInput& suspension_travel1 = mcal::suspension_travel1;
shared::periph::AnalogInput& suspension_travel2 = mcal::suspension_travel2;
shared::periph::AnalogInput& wheel_speed_front_left =
    mcal::wheel_speed_front_left;
shared::periph::AnalogInput& wheel_speed_front_right =
    mcal::wheel_speed_front_right;
shared::periph::AnalogInput& wheel_speed_rear_left =
    mcal::wheel_speed_rear_left;
shared::periph::AnalogInput& wheel_speed_rear_right =
    mcal::wheel_speed_rear_right;

// =========== Driver Control ==============================
shared::periph::InvertedDigitalInput start_button_inv{mcal::start_button};
shared::periph::DigitalInput& start_button = start_button_inv;

shared::periph::AnalogInput& steering_angle_sensor =
    mcal::steering_angle_sensor;
shared::periph::AnalogInput& brake_pressure_sensor1 =
    mcal::brake_pressure_sensor1;
shared::periph::AnalogInput& brake_pressure_sensor2 =
    mcal::brake_pressure_sensor2;
shared::periph::AnalogInput& accel_pedal_sensor1 = mcal::accel_pedal_sensor1;
shared::periph::AnalogInput& accel_pedal_sensor2 = mcal::accel_pedal_sensor2;

// =========== Outputs =====================================
shared::periph::DigitalOutput& dashboard_power_en = mcal::dashboard_power_en;
shared::periph::DigitalOutput& hvil_led_en = mcal::hvil_led_en;
shared::periph::DigitalOutput& rtds_en = mcal::rtds_en;
shared::periph::DigitalOutput& brake_light_en = mcal::brake_light_en;
shared::periph::DigitalOutput& status_led_en = mcal::status_led_en;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;

void Initialize() {
    SystemClock_Config();
    HAL_Init();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN3_Init();

    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
}

int GetTickMs() {
    return HAL_GetTick();
}

void DelayMs(int ms) {
    HAL_Delay(ms);
}

}  // namespace bindings
