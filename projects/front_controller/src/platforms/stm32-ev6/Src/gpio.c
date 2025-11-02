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
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, DEBUG_LED_Pin | DASHBOARD_HSD_EN_Pin,
                      GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOF, IMD_FAULT_LED_EN_Pin | BMS_FAULT_LED_EN_Pin,
                      GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(RTDS_EN_GPIO_Port, RTDS_EN_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : WHEEL_SPEED_RIGHT_B_BUFFERED_Pin BSPD_FAULT_Pin */
    GPIO_InitStruct.Pin = WHEEL_SPEED_RIGHT_B_BUFFERED_Pin | BSPD_FAULT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pin : WHEEL_SPEED_RIGHT_A_BUFFERED_Pin */
    GPIO_InitStruct.Pin = WHEEL_SPEED_RIGHT_A_BUFFERED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WHEEL_SPEED_RIGHT_A_BUFFERED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : DEBUG_LED_Pin DASHBOARD_HSD_EN_Pin */
    GPIO_InitStruct.Pin = DEBUG_LED_Pin | DASHBOARD_HSD_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : WHEEL_SPEED_LEFT_B_BUFFERED_Pin */
    GPIO_InitStruct.Pin = WHEEL_SPEED_LEFT_B_BUFFERED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WHEEL_SPEED_LEFT_B_BUFFERED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : WHEEL_SPEED_LEFT_A_BUFFERED_Pin */
    GPIO_InitStruct.Pin = WHEEL_SPEED_LEFT_A_BUFFERED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WHEEL_SPEED_LEFT_A_BUFFERED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : IMD_FAULT_LED_EN_Pin BMS_FAULT_LED_EN_Pin */
    GPIO_InitStruct.Pin = IMD_FAULT_LED_EN_Pin | BMS_FAULT_LED_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pin : RTDS_EN_Pin */
    GPIO_InitStruct.Pin = RTDS_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RTDS_EN_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
