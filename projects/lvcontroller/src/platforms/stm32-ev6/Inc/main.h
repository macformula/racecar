/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HSD3_I_SENSE_EN_Pin GPIO_PIN_5
#define HSD3_I_SENSE_EN_GPIO_Port GPIOE
#define HSD3_SEL_Pin GPIO_PIN_6
#define HSD3_SEL_GPIO_Port GPIOE
#define HSD6_I_SENSE_EN_Pin GPIO_PIN_14
#define HSD6_I_SENSE_EN_GPIO_Port GPIOC
#define HSD6_SEL_Pin GPIO_PIN_15
#define HSD6_SEL_GPIO_Port GPIOC
#define ANALOG_CURRENT_SENSE_Pin GPIO_PIN_3
#define ANALOG_CURRENT_SENSE_GPIO_Port GPIOF
#define ANALOG_VOLTAGE_SENSE_Pin GPIO_PIN_4
#define ANALOG_VOLTAGE_SENSE_GPIO_Port GPIOF
#define LV_VOLTAGE_SENSE_Pin GPIO_PIN_5
#define LV_VOLTAGE_SENSE_GPIO_Port GPIOF
#define HSD6_I_SENSE_Pin GPIO_PIN_7
#define HSD6_I_SENSE_GPIO_Port GPIOF
#define HSD3_I_SENSE_Pin GPIO_PIN_8
#define HSD3_I_SENSE_GPIO_Port GPIOF
#define HSD4_I_SENSE_Pin GPIO_PIN_9
#define HSD4_I_SENSE_GPIO_Port GPIOF
#define HSD5_I_SENSE_Pin GPIO_PIN_10
#define HSD5_I_SENSE_GPIO_Port GPIOF
#define TSSI_RED_SIG_Pin GPIO_PIN_2
#define TSSI_RED_SIG_GPIO_Port GPIOC
#define TSSI_GN_SIG_Pin GPIO_PIN_3
#define TSSI_GN_SIG_GPIO_Port GPIOC
#define FAN_PWM_INVERT_Pin GPIO_PIN_0
#define FAN_PWM_INVERT_GPIO_Port GPIOA
#define STP4_SIG_Pin GPIO_PIN_4
#define STP4_SIG_GPIO_Port GPIOC
#define STP3_SIG_Pin GPIO_PIN_5
#define STP3_SIG_GPIO_Port GPIOC
#define HSD1_I_SENSE_Pin GPIO_PIN_0
#define HSD1_I_SENSE_GPIO_Port GPIOB
#define HSD2_I_SENSE_Pin GPIO_PIN_1
#define HSD2_I_SENSE_GPIO_Port GPIOB
#define HSD4_I_SENSE_EN_Pin GPIO_PIN_12
#define HSD4_I_SENSE_EN_GPIO_Port GPIOF
#define HSD4_SEL_Pin GPIO_PIN_13
#define HSD4_SEL_GPIO_Port GPIOF
#define HSD5_I_SENSE_EN_Pin GPIO_PIN_14
#define HSD5_I_SENSE_EN_GPIO_Port GPIOF
#define HSD5_SEL_Pin GPIO_PIN_15
#define HSD5_SEL_GPIO_Port GPIOF
#define POWERTRAIN_FAN1_EN_Pin GPIO_PIN_0
#define POWERTRAIN_FAN1_EN_GPIO_Port GPIOG
#define IMU_GPS_EN_Pin GPIO_PIN_1
#define IMU_GPS_EN_GPIO_Port GPIOG
#define POWERTRAIN_PUMP1_EN_Pin GPIO_PIN_14
#define POWERTRAIN_PUMP1_EN_GPIO_Port GPIOE
#define POWERTRAIN_PUMP2_EN_Pin GPIO_PIN_15
#define POWERTRAIN_PUMP2_EN_GPIO_Port GPIOE
#define HSD1_I_SENSE_EN_Pin GPIO_PIN_10
#define HSD1_I_SENSE_EN_GPIO_Port GPIOB
#define HSD1_SEL0_Pin GPIO_PIN_11
#define HSD1_SEL0_GPIO_Port GPIOB
#define HSD1_SEL1_Pin GPIO_PIN_12
#define HSD1_SEL1_GPIO_Port GPIOB
#define HSD2_I_SENSE_EN_Pin GPIO_PIN_14
#define HSD2_I_SENSE_EN_GPIO_Port GPIOB
#define HSD2_SEL_Pin GPIO_PIN_15
#define HSD2_SEL_GPIO_Port GPIOB
#define IMD_FAULT_STATUS_Pin GPIO_PIN_8
#define IMD_FAULT_STATUS_GPIO_Port GPIOD
#define BMS_FAULT_STATUS_Pin GPIO_PIN_9
#define BMS_FAULT_STATUS_GPIO_Port GPIOD
#define FRONT_CONTROLLER_EN_Pin GPIO_PIN_2
#define FRONT_CONTROLLER_EN_GPIO_Port GPIOG
#define MOTOR_CONTROLLER_EN_Pin GPIO_PIN_3
#define MOTOR_CONTROLLER_EN_GPIO_Port GPIOG
#define POWERTRAIN_FAN2_EN_Pin GPIO_PIN_4
#define POWERTRAIN_FAN2_EN_GPIO_Port GPIOG
#define TSSI_EN_Pin GPIO_PIN_5
#define TSSI_EN_GPIO_Port GPIOG
#define SHUTDOWN_CIRCUIT_EN_Pin GPIO_PIN_6
#define SHUTDOWN_CIRCUIT_EN_GPIO_Port GPIOG
#define MOTOR_CONTROLLER_PRECHARGE_EN_Pin GPIO_PIN_7
#define MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port GPIOG
#define ACCUMULATOR_EN_Pin GPIO_PIN_9
#define ACCUMULATOR_EN_GPIO_Port GPIOG
#define BRAKE_LIGHT_EN_Pin GPIO_PIN_10
#define BRAKE_LIGHT_EN_GPIO_Port GPIOG
#define VICOR_EN_Pin GPIO_PIN_11
#define VICOR_EN_GPIO_Port GPIOG
#define MOTOR_CONTROLLER_SWITCH_EN_Pin GPIO_PIN_12
#define MOTOR_CONTROLLER_SWITCH_EN_GPIO_Port GPIOG
#define DCDC_SNS_SEL_Pin GPIO_PIN_14
#define DCDC_SNS_SEL_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
