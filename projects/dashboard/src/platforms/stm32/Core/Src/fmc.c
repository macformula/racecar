/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : FMC.c
 * Description        : This file provides code for the configuration
 *                      of the FMC peripheral.
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
#include "fmc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SDRAM_HandleTypeDef hsdram1;

/* FMC initialization function */
void MX_FMC_Init(void) {
    /* USER CODE BEGIN FMC_Init 0 */

    /* USER CODE END FMC_Init 0 */

    FMC_SDRAM_TimingTypeDef SdramTiming = {0};

    /* USER CODE BEGIN FMC_Init 1 */

    /* USER CODE END FMC_Init 1 */

    /** Perform the SDRAM1 memory initialization sequence
     */
    hsdram1.Instance = FMC_SDRAM_DEVICE;
    /* hsdram1.Init */
    hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
    /* SdramTiming */
    SdramTiming.LoadToActiveDelay = 2;
    SdramTiming.ExitSelfRefreshDelay = 7;
    SdramTiming.SelfRefreshTime = 4;
    SdramTiming.RowCycleDelay = 7;
    SdramTiming.WriteRecoveryTime = 3;
    SdramTiming.RPDelay = 2;
    SdramTiming.RCDDelay = 2;

    if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE BEGIN FMC_Init 2 */

    /* USER CODE END FMC_Init 2 */
}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void) {
    /* USER CODE BEGIN FMC_MspInit 0 */

    /* USER CODE END FMC_MspInit 0 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (FMC_Initialized) {
        return;
    }
    FMC_Initialized = 1;

    /* Peripheral clock enable */
    __HAL_RCC_FMC_CLK_ENABLE();

    /** FMC GPIO Configuration
    PE1   ------> FMC_NBL1
    PE0   ------> FMC_NBL0
    PG15   ------> FMC_SDNCAS
    PD0   ------> FMC_D2
    PI4   ------> FMC_NBL2
    PD1   ------> FMC_D3
    PI3   ------> FMC_D27
    PI2   ------> FMC_D26
    PF0   ------> FMC_A0
    PI5   ------> FMC_NBL3
    PI7   ------> FMC_D29
    PI10   ------> FMC_D31
    PI6   ------> FMC_D28
    PH15   ------> FMC_D23
    PI1   ------> FMC_D25
    PF1   ------> FMC_A1
    PI9   ------> FMC_D30
    PH13   ------> FMC_D21
    PH14   ------> FMC_D22
    PI0   ------> FMC_D24
    PF2   ------> FMC_A2
    PF3   ------> FMC_A3
    PG8   ------> FMC_SDCLK
    PF4   ------> FMC_A4
    PH3   ------> FMC_SDNE0
    PF5   ------> FMC_A5
    PH2   ------> FMC_SDCKE0
    PD15   ------> FMC_D1
    PD10   ------> FMC_D15
    PD14   ------> FMC_D0
    PD9   ------> FMC_D14
    PD8   ------> FMC_D13
    PC0   ------> FMC_SDNWE
    PF12   ------> FMC_A6
    PG1   ------> FMC_A11
    PF15   ------> FMC_A9
    PH12   ------> FMC_D20
    PF13   ------> FMC_A7
    PG0   ------> FMC_A10
    PE8   ------> FMC_D5
    PG5   ------> FMC_BA1
    PG4   ------> FMC_BA0
    PH9   ------> FMC_D17
    PH11   ------> FMC_D19
    PF14   ------> FMC_A8
    PF11   ------> FMC_SDNRAS
    PE9   ------> FMC_D6
    PE11   ------> FMC_D8
    PE14   ------> FMC_D11
    PH8   ------> FMC_D16
    PH10   ------> FMC_D18
    PE7   ------> FMC_D4
    PE10   ------> FMC_D7
    PE12   ------> FMC_D9
    PE15   ------> FMC_D12
    PE13   ------> FMC_D10
    */
    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = FMC_NBL1_Pin | FMC_NBL0_Pin | D5_Pin | D6_Pin |
                          D8_Pin | D11_Pin | D4_Pin | D7_Pin | D9_Pin |
                          D12_Pin | D10_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin =
        SDNCAS_Pin | SDCLK_Pin | A11_Pin | A10_Pin | GPIO_PIN_5 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin =
        D2_Pin | D3_Pin | D1_Pin | D15_Pin | D0_Pin | D14_Pin | D13_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = FMC_NBL2_Pin | D27_Pin | D26_Pin | FMC_NBL3_Pin |
                          D29_Pin | D31_Pin | D28_Pin | D25_Pin | D30_Pin |
                          D24_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = A0_Pin | A1_Pin | A2_Pin | A3_Pin | A4_Pin | A5_Pin |
                          A6_Pin | A9_Pin | A7_Pin | A8_Pin |
                          SDNMT48LC4M32B2B5_6A_RAS_RAS___Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = D23_Pin | D21_Pin | D22_Pin | SDNE0_Pin | SDCKE0_Pin |
                          D20_Pin | D17_Pin | D19_Pin | D16_Pin | D18_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = SDNWE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(SDNWE_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN FMC_MspInit 1 */

    /* USER CODE END FMC_MspInit 1 */
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* sdramHandle) {
    /* USER CODE BEGIN SDRAM_MspInit 0 */

    /* USER CODE END SDRAM_MspInit 0 */
    HAL_FMC_MspInit();
    /* USER CODE BEGIN SDRAM_MspInit 1 */

    /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void) {
    /* USER CODE BEGIN FMC_MspDeInit 0 */

    /* USER CODE END FMC_MspDeInit 0 */
    if (FMC_DeInitialized) {
        return;
    }
    FMC_DeInitialized = 1;
    /* Peripheral clock enable */
    __HAL_RCC_FMC_CLK_DISABLE();

    /** FMC GPIO Configuration
    PE1   ------> FMC_NBL1
    PE0   ------> FMC_NBL0
    PG15   ------> FMC_SDNCAS
    PD0   ------> FMC_D2
    PI4   ------> FMC_NBL2
    PD1   ------> FMC_D3
    PI3   ------> FMC_D27
    PI2   ------> FMC_D26
    PF0   ------> FMC_A0
    PI5   ------> FMC_NBL3
    PI7   ------> FMC_D29
    PI10   ------> FMC_D31
    PI6   ------> FMC_D28
    PH15   ------> FMC_D23
    PI1   ------> FMC_D25
    PF1   ------> FMC_A1
    PI9   ------> FMC_D30
    PH13   ------> FMC_D21
    PH14   ------> FMC_D22
    PI0   ------> FMC_D24
    PF2   ------> FMC_A2
    PF3   ------> FMC_A3
    PG8   ------> FMC_SDCLK
    PF4   ------> FMC_A4
    PH3   ------> FMC_SDNE0
    PF5   ------> FMC_A5
    PH2   ------> FMC_SDCKE0
    PD15   ------> FMC_D1
    PD10   ------> FMC_D15
    PD14   ------> FMC_D0
    PD9   ------> FMC_D14
    PD8   ------> FMC_D13
    PC0   ------> FMC_SDNWE
    PF12   ------> FMC_A6
    PG1   ------> FMC_A11
    PF15   ------> FMC_A9
    PH12   ------> FMC_D20
    PF13   ------> FMC_A7
    PG0   ------> FMC_A10
    PE8   ------> FMC_D5
    PG5   ------> FMC_BA1
    PG4   ------> FMC_BA0
    PH9   ------> FMC_D17
    PH11   ------> FMC_D19
    PF14   ------> FMC_A8
    PF11   ------> FMC_SDNRAS
    PE9   ------> FMC_D6
    PE11   ------> FMC_D8
    PE14   ------> FMC_D11
    PH8   ------> FMC_D16
    PH10   ------> FMC_D18
    PE7   ------> FMC_D4
    PE10   ------> FMC_D7
    PE12   ------> FMC_D9
    PE15   ------> FMC_D12
    PE13   ------> FMC_D10
    */

    HAL_GPIO_DeInit(GPIOE, FMC_NBL1_Pin | FMC_NBL0_Pin | D5_Pin | D6_Pin |
                               D8_Pin | D11_Pin | D4_Pin | D7_Pin | D9_Pin |
                               D12_Pin | D10_Pin);

    HAL_GPIO_DeInit(GPIOG, SDNCAS_Pin | SDCLK_Pin | A11_Pin | A10_Pin |
                               GPIO_PIN_5 | GPIO_PIN_4);

    HAL_GPIO_DeInit(
        GPIOD, D2_Pin | D3_Pin | D1_Pin | D15_Pin | D0_Pin | D14_Pin | D13_Pin);

    HAL_GPIO_DeInit(GPIOI, FMC_NBL2_Pin | D27_Pin | D26_Pin | FMC_NBL3_Pin |
                               D29_Pin | D31_Pin | D28_Pin | D25_Pin | D30_Pin |
                               D24_Pin);

    HAL_GPIO_DeInit(GPIOF, A0_Pin | A1_Pin | A2_Pin | A3_Pin | A4_Pin | A5_Pin |
                               A6_Pin | A9_Pin | A7_Pin | A8_Pin |
                               SDNMT48LC4M32B2B5_6A_RAS_RAS___Pin);

    HAL_GPIO_DeInit(GPIOH, D23_Pin | D21_Pin | D22_Pin | SDNE0_Pin |
                               SDCKE0_Pin | D20_Pin | D17_Pin | D19_Pin |
                               D16_Pin | D18_Pin);

    HAL_GPIO_DeInit(SDNWE_GPIO_Port, SDNWE_Pin);

    /* USER CODE BEGIN FMC_MspDeInit 1 */

    /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* sdramHandle) {
    /* USER CODE BEGIN SDRAM_MspDeInit 0 */

    /* USER CODE END SDRAM_MspDeInit 0 */
    HAL_FMC_MspDeInit();
    /* USER CODE BEGIN SDRAM_MspDeInit 1 */

    /* USER CODE END SDRAM_MspDeInit 1 */
}
/**
 * @}
 */

/**
 * @}
 */
