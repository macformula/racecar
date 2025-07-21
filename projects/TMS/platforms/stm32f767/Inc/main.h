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
#define SENS_2_UC_IN_CHANNEL ADC_CHANNEL_11
#define SENS_6_UC_IN_CHANNEL ADC_CHANNEL_15
#define SENS_3_UC_IN_CHANNEL ADC_CHANNEL_12
#define SENS_4_UC_IN_CHANNEL ADC_CHANNEL_13
#define SENS_5_UC_IN_CHANNEL ADC_CHANNEL_14
#define SENS_1_UC_IN_CHANNEL ADC_CHANNEL_10
#define DEBUG_LED1_Pin GPIO_PIN_2
#define DEBUG_LED1_GPIO_Port GPIOE
#define SENS_1_UC_IN_Pin GPIO_PIN_0
#define SENS_1_UC_IN_GPIO_Port GPIOC
#define SENS_2_UC_IN_Pin GPIO_PIN_1
#define SENS_2_UC_IN_GPIO_Port GPIOC
#define SENS_3_UC_IN_Pin GPIO_PIN_2
#define SENS_3_UC_IN_GPIO_Port GPIOC
#define SENS_4_UC_IN_Pin GPIO_PIN_3
#define SENS_4_UC_IN_GPIO_Port GPIOC
#define DEBUG_LED2_Pin GPIO_PIN_3
#define DEBUG_LED2_GPIO_Port GPIOA
#define SENS_5_UC_IN_Pin GPIO_PIN_4
#define SENS_5_UC_IN_GPIO_Port GPIOC
#define SENS_6_UC_IN_Pin GPIO_PIN_5
#define SENS_6_UC_IN_GPIO_Port GPIOC
#define NUCLEO_RED_LED_Pin GPIO_PIN_14
#define NUCLEO_RED_LED_GPIO_Port GPIOB
#define FAN_PWM_Pin GPIO_PIN_6
#define FAN_PWM_GPIO_Port GPIOB
#define NUCLEO_BLUE_LED_Pin GPIO_PIN_7
#define NUCLEO_BLUE_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
