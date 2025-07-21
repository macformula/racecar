/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, TSSI_RED_SIG_Pin | TSSI_GN_SIG_Pin,
                      GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(
        GPIOG,
        POWERTRAIN_FAN1_EN_Pin | IMU_GPS_EN_Pin | FRONT_CONTROLLER_EN_Pin |
            MOTOR_CONTROLLER_EN_Pin | POWERTRAIN_FAN2_EN_Pin | TSSI_EN_Pin |
            SHUTDOWN_CIRCUIT_EN_Pin | MOTOR_CONTROLLER_PRECHARGE_EN_Pin |
            DCDC_EN_Pin | ACCUMULATOR_EN_Pin | BRAKE_LIGHT_EN_Pin |
            MOTOR_CONTROLLER_SWITCH_EN_Pin | DCDC_SNS_SEL_Pin,
        GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, POWERTRAIN_PUMP1_EN_Pin | POWERTRAIN_PUMP2_EN_Pin,
                      GPIO_PIN_RESET);

    /*Configure GPIO pins : TSSI_RED_SIG_Pin TSSI_GN_SIG_Pin */
    GPIO_InitStruct.Pin = TSSI_RED_SIG_Pin | TSSI_GN_SIG_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : POWERTRAIN_FAN1_EN_Pin IMU_GPS_EN_Pin
       FRONT_CONTROLLER_EN_Pin MOTOR_CONTROLLER_EN_Pin POWERTRAIN_FAN2_EN_Pin
       TSSI_EN_Pin SHUTDOWN_CIRCUIT_EN_Pin MOTOR_CONTROLLER_PRECHARGE_EN_Pin
                             DCDC_EN_Pin ACCUMULATOR_EN_Pin BRAKE_LIGHT_EN_Pin
       MOTOR_CONTROLLER_SWITCH_EN_Pin DCDC_SNS_SEL_Pin */
    GPIO_InitStruct.Pin =
        POWERTRAIN_FAN1_EN_Pin | IMU_GPS_EN_Pin | FRONT_CONTROLLER_EN_Pin |
        MOTOR_CONTROLLER_EN_Pin | POWERTRAIN_FAN2_EN_Pin | TSSI_EN_Pin |
        SHUTDOWN_CIRCUIT_EN_Pin | MOTOR_CONTROLLER_PRECHARGE_EN_Pin |
        DCDC_EN_Pin | ACCUMULATOR_EN_Pin | BRAKE_LIGHT_EN_Pin |
        MOTOR_CONTROLLER_SWITCH_EN_Pin | DCDC_SNS_SEL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /*Configure GPIO pins : POWERTRAIN_PUMP1_EN_Pin POWERTRAIN_PUMP2_EN_Pin */
    GPIO_InitStruct.Pin = POWERTRAIN_PUMP1_EN_Pin | POWERTRAIN_PUMP2_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : IMD_FAULT_STATUS_Pin BMS_FAULT_STATUS_Pin */
    GPIO_InitStruct.Pin = IMD_FAULT_STATUS_Pin | BMS_FAULT_STATUS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
