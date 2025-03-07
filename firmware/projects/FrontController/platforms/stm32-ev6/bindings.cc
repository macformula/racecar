#include "../../bindings.hpp"
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/analog_input.hpp"
#include "mcal/stm32f767/periph/can.hpp"
#include "mcal/stm32f767/periph/gpio.hpp"
#include "stm32f7xx_hal.h"

extern "C" {
// This requires extern since it is not declared in a header, only defined in
// cubemx/../main.c
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

// =========== CAN =========================================
CanBase veh_can_base{&hcan2};
CanBase pt_can_base{&hcan1};

// =========== Vehicle Dynamics Sensors ====================
AnalogInput suspension_travel1{&hadc1, ADC_CHANNEL_8};
AnalogInput suspension_travel2{&hadc1, ADC_CHANNEL_9};
AnalogInput wheel_speed_front_left{&hadc1, ADC_CHANNEL_15};
AnalogInput wheel_speed_front_right{&hadc3, ADC_CHANNEL_14};
AnalogInput wheel_speed_rear_left{&hadc1, ADC_CHANNEL_14};
AnalogInput wheel_speed_rear_right{&hadc3, ADC_CHANNEL_9};

// =========== Driver Control ==============================
AnalogInput steering_angle_sensor{&hadc1, ADC_CHANNEL_2};
AnalogInput accel_pedal_sensor1{&hadc3, ADC_CHANNEL_6};
AnalogInput accel_pedal_sensor2{&hadc3, ADC_CHANNEL_7};
AnalogInput brake_pressure_sensor{&hadc1, ADC_CHANNEL_12};

// =========== Status Monitors =============================
AnalogInput precharge_monitor{&hadc1, ADC_CHANNEL_1};
DigitalInput bspd_fault{BSPD_FAULT_GPIO_Port, BSPD_FAULT_Pin};

// =========== Outputs =====================================
DigitalOutput dashboard_power_en{DASHBOARD_HSD_EN_GPIO_Port,
                                 DASHBOARD_HSD_EN_Pin};
DigitalOutput imd_fault_led_en{IMD_FAULT_LED_EN_GPIO_Port,
                               IMD_FAULT_LED_EN_Pin};
DigitalOutput bms_fault_led_en{BMS_FAULT_LED_EN_GPIO_Port,
                               BMS_FAULT_LED_EN_Pin};
DigitalOutput ready_to_drive_sig_en{RTDS_EN_GPIO_Port, RTDS_EN_Pin};
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
shared::periph::AnalogInput& steering_angle_sensor =
    mcal::steering_angle_sensor;
shared::periph::AnalogInput& accel_pedal_sensor1 = mcal::accel_pedal_sensor1;
shared::periph::AnalogInput& accel_pedal_sensor2 = mcal::accel_pedal_sensor2;
shared::periph::AnalogInput& brake_pressure_sensor =
    mcal::brake_pressure_sensor;

// =========== Status Monitors =============================
shared::periph::AnalogInput& precharge_monitor = mcal::precharge_monitor;
shared::periph::DigitalInput& bspd_fault = mcal::bspd_fault;

// =========== Outputs =====================================
shared::periph::DigitalOutput& dashboard_power_en = mcal::dashboard_power_en;
shared::periph::DigitalOutput& imd_fault_led_en = mcal::imd_fault_led_en;
shared::periph::DigitalOutput& bms_fault_led_en = mcal::bms_fault_led_en;
shared::periph::DigitalOutput& ready_to_drive_sig_en =
    mcal::ready_to_drive_sig_en;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;

void Initialize() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();

    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
}

int GetTickMs() {
    return HAL_GetTick();
}

void DelayMs(int ms) {
    HAL_Delay(ms);
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

}  // namespace bindings
