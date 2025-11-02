/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#define WHEEL_SPEED_RIGHT_B_BUFFERED_Pin GPIO_PIN_3
#define WHEEL_SPEED_RIGHT_B_BUFFERED_GPIO_Port GPIOF
#define WHEEL_SPEED_RIGHT_A_BUFFERED_Pin GPIO_PIN_4
#define WHEEL_SPEED_RIGHT_A_BUFFERED_GPIO_Port GPIOF
#define WHEEL_SPEED_RIGHT_A_BUFFERED_EXTI_IRQn EXTI4_IRQn
#define APPS1_BUFFERED_Pin GPIO_PIN_8
#define APPS1_BUFFERED_GPIO_Port GPIOF
#define APPS2_BUFFERED_Pin GPIO_PIN_9
#define APPS2_BUFFERED_GPIO_Port GPIOF
#define BSPD_FAULT_Pin GPIO_PIN_10
#define BSPD_FAULT_GPIO_Port GPIOF
#define BPPS_BUFFERED_Pin GPIO_PIN_2
#define BPPS_BUFFERED_GPIO_Port GPIOC
#define PRECHARGE_MONITOR_Pin GPIO_PIN_1
#define PRECHARGE_MONITOR_GPIO_Port GPIOA
#define STEERING_ANGLE_SIG_Pin GPIO_PIN_2
#define STEERING_ANGLE_SIG_GPIO_Port GPIOA
#define DEBUG_LED_Pin GPIO_PIN_3
#define DEBUG_LED_GPIO_Port GPIOA
#define DASHBOARD_HSD_EN_Pin GPIO_PIN_6
#define DASHBOARD_HSD_EN_GPIO_Port GPIOA
#define WHEEL_SPEED_LEFT_B_BUFFERED_Pin GPIO_PIN_4
#define WHEEL_SPEED_LEFT_B_BUFFERED_GPIO_Port GPIOC
#define WHEEL_SPEED_LEFT_A_BUFFERED_Pin GPIO_PIN_5
#define WHEEL_SPEED_LEFT_A_BUFFERED_GPIO_Port GPIOC
#define WHEEL_SPEED_LEFT_A_BUFFERED_EXTI_IRQn EXTI9_5_IRQn
#define STP1_SIG_Pin GPIO_PIN_0
#define STP1_SIG_GPIO_Port GPIOB
#define STP2_SIG_Pin GPIO_PIN_1
#define STP2_SIG_GPIO_Port GPIOB
#define IMD_FAULT_LED_EN_Pin GPIO_PIN_14
#define IMD_FAULT_LED_EN_GPIO_Port GPIOF
#define BMS_FAULT_LED_EN_Pin GPIO_PIN_15
#define BMS_FAULT_LED_EN_GPIO_Port GPIOF
#define RTDS_EN_Pin GPIO_PIN_0
#define RTDS_EN_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
