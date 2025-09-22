/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan3;

/* CAN3 init function */
void MX_CAN3_Init(void) {
    /* USER CODE BEGIN CAN3_Init 0 */

    /* USER CODE END CAN3_Init 0 */

    /* USER CODE BEGIN CAN3_Init 1 */

    /* USER CODE END CAN3_Init 1 */
    hcan3.Instance = CAN3;
    hcan3.Init.Prescaler = 6;
    hcan3.Init.Mode = CAN_MODE_NORMAL;
    hcan3.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan3.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan3.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan3.Init.TimeTriggeredMode = DISABLE;
    hcan3.Init.AutoBusOff = ENABLE;
    hcan3.Init.AutoWakeUp = DISABLE;
    hcan3.Init.AutoRetransmission = DISABLE;
    hcan3.Init.ReceiveFifoLocked = DISABLE;
    hcan3.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan3) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN CAN3_Init 2 */

    /* USER CODE END CAN3_Init 2 */
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (canHandle->Instance == CAN3) {
        /* USER CODE BEGIN CAN3_MspInit 0 */

        /* USER CODE END CAN3_MspInit 0 */
        /* CAN3 clock enable */
        __HAL_RCC_CAN3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**CAN3 GPIO Configuration
        PB3     ------> CAN3_RX
        PB4     ------> CAN3_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_CAN3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* CAN3 interrupt Init */
        HAL_NVIC_SetPriority(CAN3_RX0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN3_RX0_IRQn);
        /* USER CODE BEGIN CAN3_MspInit 1 */

        /* USER CODE END CAN3_MspInit 1 */
    }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle) {
    if (canHandle->Instance == CAN3) {
        /* USER CODE BEGIN CAN3_MspDeInit 0 */

        /* USER CODE END CAN3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_CAN3_CLK_DISABLE();

        /**CAN3 GPIO Configuration
        PB3     ------> CAN3_RX
        PB4     ------> CAN3_TX
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4);

        /* CAN3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN3_RX0_IRQn);
        /* USER CODE BEGIN CAN3_MspDeInit 1 */

        /* USER CODE END CAN3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
