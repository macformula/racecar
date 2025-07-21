/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    dsihost.c
 * @brief   This file provides code for the configuration
 *          of the DSIHOST instances.
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
#include "dsihost.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DSI_HandleTypeDef hdsi;

/* DSIHOST init function */

void MX_DSIHOST_DSI_Init(void) {
    /* USER CODE BEGIN DSIHOST_Init 0 */

    /* USER CODE END DSIHOST_Init 0 */

    DSI_PLLInitTypeDef PLLInit = {0};
    DSI_HOST_TimeoutTypeDef HostTimeouts = {0};
    DSI_PHY_TimerTypeDef PhyTimings = {0};
    DSI_VidCfgTypeDef VidCfg = {0};

    /* USER CODE BEGIN DSIHOST_Init 1 */

    /* USER CODE END DSIHOST_Init 1 */
    hdsi.Instance = DSI;
    hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
    hdsi.Init.TXEscapeCkdiv = 4;
    hdsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    PLLInit.PLLNDIV = 125;
    PLLInit.PLLIDF = DSI_PLL_IN_DIV2;
    PLLInit.PLLODF = DSI_PLL_OUT_DIV1;
    if (HAL_DSI_Init(&hdsi, &PLLInit) != HAL_OK) {
        Error_Handler();
    }
    HostTimeouts.TimeoutCkdiv = 1;
    HostTimeouts.HighSpeedTransmissionTimeout = 0;
    HostTimeouts.LowPowerReceptionTimeout = 0;
    HostTimeouts.HighSpeedReadTimeout = 0;
    HostTimeouts.LowPowerReadTimeout = 0;
    HostTimeouts.HighSpeedWriteTimeout = 0;
    HostTimeouts.HighSpeedWritePrespMode = DSI_HS_PM_DISABLE;
    HostTimeouts.LowPowerWriteTimeout = 0;
    HostTimeouts.BTATimeout = 0;
    if (HAL_DSI_ConfigHostTimeouts(&hdsi, &HostTimeouts) != HAL_OK) {
        Error_Handler();
    }
    PhyTimings.ClockLaneHS2LPTime = 35;
    PhyTimings.ClockLaneLP2HSTime = 35;
    PhyTimings.DataLaneHS2LPTime = 35;
    PhyTimings.DataLaneLP2HSTime = 35;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 10;
    if (HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_DSI_SetLowPowerRXFilter(&hdsi, 10000) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_DSI_ConfigErrorMonitor(&hdsi, HAL_DSI_ERROR_NONE) != HAL_OK) {
        Error_Handler();
    }
    VidCfg.VirtualChannelID = 0;
    VidCfg.ColorCoding = DSI_RGB888;
    VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
    VidCfg.Mode = DSI_VID_MODE_BURST;
    VidCfg.PacketSize = 800;
    VidCfg.NumberOfChunks = 0;
    VidCfg.NullPacketSize = 0;
    VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    VidCfg.HorizontalSyncActive = 5;
    VidCfg.HorizontalBackPorch = 77;
    VidCfg.HorizontalLine = 1982;
    VidCfg.VerticalSyncActive = 120;
    VidCfg.VerticalBackPorch = 150;
    VidCfg.VerticalFrontPorch = 150;
    VidCfg.VerticalActive = 480;
    VidCfg.LPCommandEnable = DSI_LP_COMMAND_ENABLE;
    VidCfg.LPLargestPacketSize = 16;
    VidCfg.LPVACTLargestPacketSize = 0;
    VidCfg.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
    VidCfg.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
    VidCfg.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;
    VidCfg.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
    VidCfg.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
    VidCfg.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
    VidCfg.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;
    if (HAL_DSI_ConfigVideoMode(&hdsi, &VidCfg) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_DSI_SetGenericVCID(&hdsi, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN DSIHOST_Init 2 */

    /* USER CODE END DSIHOST_Init 2 */
}

void HAL_DSI_MspInit(DSI_HandleTypeDef* dsiHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (dsiHandle->Instance == DSI) {
        /* USER CODE BEGIN DSI_MspInit 0 */

        /* USER CODE END DSI_MspInit 0 */
        /* DSI clock enable */
        __HAL_RCC_DSI_CLK_ENABLE();

        __HAL_RCC_GPIOJ_CLK_ENABLE();
        /**DSIHOST GPIO Configuration
        PJ2     ------> DSIHOST_TE
        */
        GPIO_InitStruct.Pin = DSI_TE_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DSI;
        HAL_GPIO_Init(DSI_TE_GPIO_Port, &GPIO_InitStruct);

        /* DSI interrupt Init */
        HAL_NVIC_SetPriority(DSI_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DSI_IRQn);
        /* USER CODE BEGIN DSI_MspInit 1 */

        /* USER CODE END DSI_MspInit 1 */
    }
}

void HAL_DSI_MspDeInit(DSI_HandleTypeDef* dsiHandle) {
    if (dsiHandle->Instance == DSI) {
        /* USER CODE BEGIN DSI_MspDeInit 0 */

        /* USER CODE END DSI_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_DSI_CLK_DISABLE();

        /**DSIHOST GPIO Configuration
        PJ2     ------> DSIHOST_TE
        */
        HAL_GPIO_DeInit(DSI_TE_GPIO_Port, DSI_TE_Pin);

        /* DSI interrupt Deinit */
        HAL_NVIC_DisableIRQ(DSI_IRQn);
        /* USER CODE BEGIN DSI_MspDeInit 1 */

        /* USER CODE END DSI_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
