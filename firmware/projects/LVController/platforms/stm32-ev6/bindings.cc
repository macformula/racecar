/// @author Blake Freer
/// @date 2023-12-25

// cubemx files
// fw includes

// CubeMX
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/analog_input.hpp"
#include "mcal/stm32f767/periph/can.hpp"
#include "shared/periph/can.hpp"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"

// Firmware
#include "../../bindings.hpp"
#include "mcal/stm32f767/periph/gpio.hpp"
#include "mcal/stm32f767/periph/pwm.hpp"
#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
void SystemClock_Config();
}

namespace mcal {
using namespace mcal::stm32f767::periph;

// Tractive System Status Indicator
DigitalOutput tssi_en{TSSI_EN_GPIO_Port, TSSI_EN_Pin};
DigitalOutput tssi_red_signal{TSSI_RED_SIG_GPIO_Port, TSSI_RED_SIG_Pin};
DigitalOutput tssi_green_signal{TSSI_GN_SIG_GPIO_Port, TSSI_GN_SIG_Pin};
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
DigitalOutput raspberry_pi_en{RASPI_EN_GPIO_Port, RASPI_EN_Pin};
DigitalOutput shutdown_circuit_en{SHUTDOWN_CIRCUIT_EN_GPIO_Port,
                                  SHUTDOWN_CIRCUIT_EN_Pin};

// DCDC System  Measurement
DigitalOutput dcdc_en{DCDC_EN_GPIO_Port, DCDC_EN_Pin};
DigitalOutput dcdc_sense_select{DCDC_SNS_SEL_GPIO_Port, DCDC_SNS_SEL_Pin};
AnalogInput dcdc_sense{&hadc1, ADC_CHANNEL_10};

// Other IO
DigitalOutput brake_light_en{BRAKE_LIGHT_EN_GPIO_Port, BRAKE_LIGHT_EN_Pin};
AnalogInput suspension_travel3{&hadc1, ADC_CHANNEL_15};
AnalogInput suspension_travel4{&hadc1, ADC_CHANNEL_14};
CanBase veh_can_base{&hcan2};

}  // namespace mcal

namespace bindings {
using namespace shared::periph;

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
DigitalOutput& raspberry_pi_en = mcal::raspberry_pi_en;
DigitalOutput& shutdown_circuit_en = mcal::shutdown_circuit_en;

// DCDC System & Measurement
DigitalOutput& dcdc_en = mcal::dcdc_en;
DigitalOutput& dcdc_sense_select = mcal::dcdc_sense_select;
AnalogInput& dcdc_sense = mcal::dcdc_sense;

// Other IO
DigitalOutput& brake_light_en = mcal::brake_light_en;
AnalogInput& suspension_travel3 = mcal::suspension_travel3;
AnalogInput& suspension_travel4 = mcal::suspension_travel4;
CanBase& veh_can_base = mcal::veh_can_base;

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN2_Init();
    MX_TIM2_Init();

    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}

int GetTick() {
    return HAL_GetTick();
}

}  // namespace bindings