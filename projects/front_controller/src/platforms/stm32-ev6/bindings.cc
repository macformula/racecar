#include "bindings.hpp"

#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f/analog_input.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "stm32f7xx_hal.h"

int32_t left_wheel_tick_count = 0;
int32_t right_wheel_tick_count = 0;
bool right_wheel_forward = 0;
bool left_wheel_forward = 0;

extern "C" {
// This requires extern since it is not declared in a header, only defined in
// cubemx/../main.c
void SystemClock_Config();

// on rise edge if B line is low we are forward

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == WHEEL_SPEED_LEFT_A_BUFFERED_Pin) {
        left_wheel_tick_count += 1;
        // on rising edge, if B line is low we are forward
        left_wheel_forward =
            !HAL_GPIO_ReadPin(WHEEL_SPEED_LEFT_B_BUFFERED_GPIO_Port,
                              WHEEL_SPEED_RIGHT_B_BUFFERED_Pin);

    } else if (GPIO_Pin == WHEEL_SPEED_RIGHT_A_BUFFERED_Pin) {
        right_wheel_tick_count += 1;
        // on rising edge, if B line is low we are forward
        right_wheel_forward =
            !HAL_GPIO_ReadPin(WHEEL_SPEED_LEFT_B_BUFFERED_GPIO_Port,
                              WHEEL_SPEED_RIGHT_B_BUFFERED_Pin);
    }
}
}

namespace mcal {
using namespace stm32f;

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
macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;
macfe::periph::CanBase& pt_can_base = mcal::pt_can_base;

// =========== Vehicle Dynamics Sensors ====================
macfe::periph::AnalogInput& suspension_travel1 = mcal::suspension_travel1;
macfe::periph::AnalogInput& suspension_travel2 = mcal::suspension_travel2;
macfe::periph::AnalogInput& wheel_speed_front_left =
    mcal::wheel_speed_front_left;
macfe::periph::AnalogInput& wheel_speed_front_right =
    mcal::wheel_speed_front_right;
macfe::periph::AnalogInput& wheel_speed_rear_left = mcal::wheel_speed_rear_left;
macfe::periph::AnalogInput& wheel_speed_rear_right =
    mcal::wheel_speed_rear_right;

// =========== Driver Control ==============================
macfe::periph::AnalogInput& steering_angle_sensor = mcal::steering_angle_sensor;
macfe::periph::AnalogInput& accel_pedal_sensor1 = mcal::accel_pedal_sensor1;
macfe::periph::AnalogInput& accel_pedal_sensor2 = mcal::accel_pedal_sensor2;
macfe::periph::AnalogInput& brake_pressure_sensor = mcal::brake_pressure_sensor;

// =========== Status Monitors =============================
macfe::periph::AnalogInput& precharge_monitor = mcal::precharge_monitor;
macfe::periph::DigitalInput& bspd_fault = mcal::bspd_fault;

// =========== Outputs =====================================
macfe::periph::DigitalOutput& dashboard_power_en = mcal::dashboard_power_en;
macfe::periph::DigitalOutput& imd_fault_led_en = mcal::imd_fault_led_en;
macfe::periph::DigitalOutput& bms_fault_led_en = mcal::bms_fault_led_en;
macfe::periph::DigitalOutput& ready_to_drive_sig_en =
    mcal::ready_to_drive_sig_en;
macfe::periph::DigitalOutput& debug_led = mcal::debug_led;

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

int32_t GetLeftWheelTick() {
    return left_wheel_tick_count;
}

int32_t GetRightWheelTick() {
    return right_wheel_tick_count;
}

bool GetLeftWheelForward() {
    return left_wheel_forward;
}

bool GetRightWheelForward() {
    return right_wheel_forward;
}

}  // namespace bindings