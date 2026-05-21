/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
// fw includes

// CubeMX
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"

// Firmware
#include "bindings.hpp"
#include "mcal/stm32f/analog_input.hpp"
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"
#include "mcal/stm32f/pwm.hpp"
#include "periph/can.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
// void MPU_Config();
void SystemClock_Config();
}

namespace mcal {
using namespace mcal::stm32f;

// Tractive System Status Indicator
DigitalOutput tssi_en{TSSI_EN_GPIO_Port, TSSI_EN_Pin};
DigitalOutput tssi_red_signal_n{TSSI_RED_SIG_GPIO_Port, TSSI_RED_SIG_Pin};
DigitalOutput tssi_green_signal_n{TSSI_GN_SIG_GPIO_Port, TSSI_GN_SIG_Pin};

// Both red and green are inverted logic
macfe::periph::InvertedDigitalOutput tssi_red_signal{tssi_red_signal_n};
macfe::periph::InvertedDigitalOutput tssi_green_signal{tssi_green_signal_n};

DigitalInput imd_fault{IMD_FAULT_STATUS_GPIO_Port, IMD_FAULT_STATUS_Pin};
DigitalInput bms_fault{BMS_FAULT_STATUS_GPIO_Port, BMS_FAULT_STATUS_Pin};

// Powertrain Cooling
DigitalOutput powertrain_pump1_en{POWERTRAIN_PUMP1_EN_GPIO_Port,
                                  POWERTRAIN_PUMP1_EN_Pin};
DigitalOutput powertrain_pump2_en{POWERTRAIN_PUMP2_EN_GPIO_Port,
                                  POWERTRAIN_PUMP2_EN_Pin};
DigitalOutput powertrain_fan1_en{POWERTRAIN_FAN1_EN_GPIO_Port,
                                 POWERTRAIN_FAN1_EN_Pin};
DigitalOutput powertrain_fan2_en{POWERTRAIN_FAN2_EN_GPIO_Port,
                                 POWERTRAIN_FAN2_EN_Pin};
PWMOutput powertrain_fan_pwm{&htim2, HAL_TIM_ACTIVE_CHANNEL_1};

// Motor Controller (i.e. Inverters)
DigitalOutput motor_controller_en{MOTOR_CONTROLLER_EN_GPIO_Port,
                                  MOTOR_CONTROLLER_EN_Pin};
DigitalOutput motor_ctrl_precharge_en{MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port,
                                      MOTOR_CONTROLLER_PRECHARGE_EN_Pin};
DigitalOutput motor_ctrl_switch_en{MOTOR_CONTROLLER_SWITCH_EN_GPIO_Port,
                                   MOTOR_CONTROLLER_SWITCH_EN_Pin};

// Subsystems
DigitalOutput accumulator_en{ACCUMULATOR_EN_GPIO_Port, ACCUMULATOR_EN_Pin};
DigitalOutput front_controller_en{FRONT_CONTROLLER_EN_GPIO_Port,
                                  FRONT_CONTROLLER_EN_Pin};
DigitalOutput imu_gps_en{IMU_GPS_EN_GPIO_Port, IMU_GPS_EN_Pin};
DigitalOutput shutdown_circuit_en{SHUTDOWN_CIRCUIT_EN_GPIO_Port,
                                  SHUTDOWN_CIRCUIT_EN_Pin};

// DCDC System  Measurement
// DigitalOutput dcdc_sense_select{DCDC_SNS_SEL_GPIO_Port, DCDC_SNS_SEL_Pin};

DigitalOutput vicor_en{VICOR_EN_GPIO_Port, VICOR_EN_Pin};
// Analog Sensing (Rev 4)
AnalogInput lv_battery{&hadc3, ADC_CHANNEL_15};
AnalogInput bus_voltage{&hadc3, ADC_CHANNEL_14};
AnalogInput bus_current{&hadc3, ADC_CHANNEL_9};

// HSD
AnalogInput hsd1_isense{&hadc1, ADC_CHANNEL_8};
DigitalOutput hsd1_isense_en{HSD1_I_SENSE_EN_GPIO_Port, HSD1_I_SENSE_EN_Pin};
DigitalOutput hsd1_sel0{HSD1_SEL0_GPIO_Port, HSD1_SEL0_Pin};
DigitalOutput hsd1_sel1{HSD1_SEL1_GPIO_Port, HSD1_SEL1_Pin};

hsd::HSD4Channel hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};
hsd::HSD2Channel hsd2{hsd2_isense, hsd2_isense_en, hsd2_sel};

AnalogInput hsd2_isense{&hadc1, ADC_CHANNEL_9};
DigitalOutput hsd2_isense_en{HSD2_I_SENSE_EN_GPIO_Port, HSD2_I_SENSE_EN_Pin};
DigitalOutput hsd2_sel{HSD2_SEL_GPIO_Port, HSD2_SEL_Pin};

AnalogInput hsd3_isense{&hadc3, ADC_CHANNEL_6};
DigitalOutput hsd3_isense_en{HSD3_I_SENSE_EN_GPIO_Port, HSD3_I_SENSE_EN_Pin};
DigitalOutput hsd3_sel{HSD3_SEL_GPIO_Port, HSD3_SEL_Pin};

AnalogInput hsd4_isense{&hadc3, ADC_CHANNEL_7};
DigitalOutput hsd4_isense_en{HSD4_I_SENSE_EN_GPIO_Port, HSD4_I_SENSE_EN_Pin};
DigitalOutput hsd4_sel{HSD4_SEL_GPIO_Port, HSD4_SEL_Pin};

AnalogInput hsd5_isense{&hadc3, ADC_CHANNEL_8};
DigitalOutput hsd5_isense_en{HSD5_I_SENSE_EN_GPIO_Port, HSD5_I_SENSE_EN_Pin};
DigitalOutput hsd5_sel{HSD5_SEL_GPIO_Port, HSD5_SEL_Pin};

AnalogInput hsd6_isense{&hadc3, ADC_CHANNEL_5};
DigitalOutput hsd6_isense_en{HSD6_I_SENSE_EN_GPIO_Port, HSD6_I_SENSE_EN_Pin};
DigitalOutput hsd6_sel{HSD6_SEL_GPIO_Port, HSD6_SEL_Pin};

// Other IO
DigitalOutput brake_light_en{BRAKE_LIGHT_EN_GPIO_Port, BRAKE_LIGHT_EN_Pin};
AnalogInput suspension_travel3{&hadc1, ADC_CHANNEL_15};
AnalogInput suspension_travel4{&hadc1, ADC_CHANNEL_14};
CanBase veh_can_base{&hcan2};

}  // namespace mcal

namespace bindings {
using namespace macfe::periph;

// Tractive System Status Indicator
DigitalOutput& tssi_en = mcal::tssi_en;

DigitalOutput& tssi_red_signal = mcal::tssi_red_signal;
DigitalOutput& tssi_green_signal = mcal::tssi_green_signal;
DigitalInput& imd_fault = mcal::imd_fault;
DigitalInput& bms_fault = mcal::bms_fault;

// Powertrain Cooling
DigitalOutput& powertrain_pump1_en = mcal::powertrain_pump1_en;
DigitalOutput& powertrain_pump2_en = mcal::powertrain_pump2_en;
DigitalOutput& powertrain_fan1_en = mcal::powertrain_fan1_en;
DigitalOutput& powertrain_fan2_en = mcal::powertrain_fan2_en;
PWMOutput& powertrain_fan_pwm = mcal::powertrain_fan_pwm;

// Motor Controller (i.e. Inverters)
DigitalOutput& motor_controller_en = mcal::motor_controller_en;
DigitalOutput& motor_ctrl_precharge_en = mcal::motor_ctrl_precharge_en;
DigitalOutput& motor_ctrl_switch_en = mcal::motor_ctrl_switch_en;

// Subsystems
DigitalOutput& accumulator_en = mcal::accumulator_en;
DigitalOutput& front_controller_en = mcal::front_controller_en;
DigitalOutput& imu_gps_en = mcal::imu_gps_en;
DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

DigitalOutput& vicor_en = mcal::vicor_en;
// Analog Sensing Rev 4
AnalogInput& lv_battery = mcal::lv_battery;
AnalogInput& bus_voltage = mcal::bus_voltage;
AnalogInput& bus_current = mcal::bus_current;

// HSD
AnalogInput& hsd1_isense = mcal::hsd1_isense;
DigitalOutput& hsd1_isense_en = mcal::hsd1_isense_en;
DigitalOutput& hsd1_sel0 = mcal::hsd1_sel0;
DigitalOutput& hsd1_sel1 = mcal::hsd1_sel1;

AnalogInput& hsd2_isense = mcal::hsd2_isense;
DigitalOutput& hsd2_isense_en = mcal::hsd2_isense_en;
DigitalOutput& hsd2_sel = mcal::hsd2_sel;

AnalogInput& hsd3_isense = mcal::hsd3_isense;
DigitalOutput& hsd3_isense_en = mcal::hsd3_isense_en;
DigitalOutput& hsd3_sel = mcal::hsd3_sel;

AnalogInput& hsd4_isense = mcal::hsd4_isense;
DigitalOutput& hsd4_isense_en = mcal::hsd4_isense_en;
DigitalOutput& hsd4_sel = mcal::hsd4_sel;

AnalogInput& hsd5_isense = mcal::hsd5_isense;
DigitalOutput& hsd5_isense_en = mcal::hsd5_isense_en;
DigitalOutput& hsd5_sel = mcal::hsd5_sel;

AnalogInput& hsd6_isense = mcal::hsd6_isense;
DigitalOutput& hsd6_isense_en = mcal::hsd6_isense_en;
DigitalOutput& hsd6_sel = mcal::hsd6_sel;

hsd::HSD4Channel& hsd1 = mcal::hsd1;
hsd::HSD2Channel& hsd2 = mcal::hsd2;

// Other IO
DigitalOutput& brake_light_en = mcal::brake_light_en;
AnalogInput& suspension_travel3 = mcal::suspension_travel3;
AnalogInput& suspension_travel4 = mcal::suspension_travel4;
CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    // MPU_Config();
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    MX_CAN2_Init();
    MX_TIM2_Init();

    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}

int GetTick() {
    static uint32_t first_tick = HAL_GetTick();
    return HAL_GetTick() - first_tick;
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

}  // namespace bindings