/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <string.h>

#include "can.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define METADATA_ADDRESS 0x08008000U  // Sector 1
#define APP_FLASH_START 0x08010000U   // Sector 2
#define APP_FLASH_END 0x08200000U     // Exclusive end
#define APP_ADDRESS APP_FLASH_START
#define IMAGE_VALID_MARKER 0xA5A55A5AU
CAN_HandleTypeDef* hcan_;

// STM32F767ZI single-bank mode: erase only metadata sector 1.
// Call before modifying application flash; abort the update on failure.
HAL_StatusTypeDef EraseMetadata(void) {
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_error = 0xFFFFFFFFU;

    HAL_StatusTypeDef status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_ERSERR);

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_1;
    erase.NbSectors = 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    status = HAL_FLASHEx_Erase(&erase, &sector_error);

    // Lock flash even if erasing failed. Keep interrupts enabled for HAL ticks.
    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();
    return (status == HAL_OK) ? lock_status : status;
}

HAL_StatusTypeDef SaveMetadata(uint32_t image_size, uint32_t image_crc) {
    if (image_size < 8U || image_size > (APP_FLASH_END - APP_FLASH_START)) {
        return HAL_ERROR;
    }

    // reject attempts to overwrite an existing record without erasing it.
    volatile const uint32_t* record =
        (volatile const uint32_t*)METADATA_ADDRESS;
    if (record[0] != 0xFFFFFFFFU || record[1] != 0xFFFFFFFFU ||
        record[2] != 0xFFFFFFFFU) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    status =
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, METADATA_ADDRESS, image_size);
    if (status == HAL_OK) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                   METADATA_ADDRESS + 4U, image_crc);
    }
    // commit last: a failed size/CRC write must not mark the image valid.
    if (status == HAL_OK) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                   METADATA_ADDRESS + 8U, IMAGE_VALID_MARKER);
    }

    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();
    if (status != HAL_OK) {
        return status;
    }
    if (lock_status != HAL_OK) {
        return lock_status;
    }

    return record[0] == image_size && record[1] == image_crc &&
                   record[2] == IMAGE_VALID_MARKER
               ? HAL_OK
               : HAL_ERROR;
}

void CanSetup(void) {
    hcan_ = &hcan1;
    MX_CAN1_Init();

    CAN_FilterTypeDef filter = {0};
    filter.FilterBank = (hcan_->Instance == CAN1) ? 0 : 14;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0;
    filter.FilterIdLow = 0;
    filter.FilterMaskIdHigh = 0;
    filter.FilterMaskIdLow = 0;  // zero mask accepts every CAN ID
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(hcan_, &filter) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_Start(hcan_) != HAL_OK) {
        Error_Handler();
    }
}

HAL_StatusTypeDef ReceiveFirmwareBlock(uint32_t firmware_can_id,
                                       uint8_t* buffer, uint32_t block_bytes,
                                       uint32_t timeout_ms) {
    // before receiving the firmware, we need to validate transfer by updating
    // metadata

    uint32_t receieved = 0;
    uint32_t last_rx = HAL_GetTick();

    while (receieved < block_bytes) {
        if ((uint32_t)(HAL_GetTick() - last_rx) >= timeout_ms) {
            return HAL_TIMEOUT;
        }

        if (HAL_CAN_GetRxFifoFillLevel(hcan_, CAN_RX_FIFO0) == 0U) {
            continue;
        }

        CAN_RxHeaderTypeDef header;
        uint8_t data[8];

        if (HAL_CAN_GetRxMessage(hcan_, CAN_RX_FIFO0, &header, data) !=
            HAL_OK) {
            return HAL_ERROR;
        }

        if (header.IDE != CAN_ID_STD || header.RTR != CAN_RTR_DATA ||
            header.StdId != firmware_can_id) {
            continue;
        }

        if (header.DLC == 0U || header.DLC > 8U) {
            return HAL_ERROR;
        }

        uint32_t remaining = block_bytes - receieved;
        uint32_t count = header.DLC;

        if (count > remaining) {
            count = remaining;
        }

        memcpy(&buffer[receieved], data, count);
        receieved += count;
        last_rx = HAL_GetTick();
    }

    return HAL_OK;
}

HAL_StatusTypeDef WriteFirmwareBlock(uint32_t address, const uint8_t* buffer,
                                     uint32_t length) {
    // address must be 4 byte aligned with FLASH_TYPEPROGRAM_WORD
    if (buffer == NULL || length == 0 || (address & 3U) != 0U ||
        address < APP_FLASH_START || address >= APP_FLASH_END ||
        length > (APP_FLASH_END - address)) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = HAL_FLASH_Unlock();

    if (status != HAL_OK) {
        return status;
    }

    for (uint32_t offset = 0; offset < length; offset += 4U) {
        uint32_t word = 0xFFFFFFFFU;
        uint32_t count = length - offset;

        if (count > 4U) count = 4U;

        memcpy(&word, &buffer[offset], count);

        status =
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + offset, word);

        if (status != HAL_OK) {
            break;
        }
    }

    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();

    return (status == HAL_OK) ? lock_status : status;
}

enum {
    FLASH_ACK_SELECTED = 0U,
    FLASH_ACK_READY = 1U,
    FLASH_ACK_BLOCK_WRITTEN = 2U
};

HAL_StatusTypeDef SendAcknowledgementBit(uint8_t status) {
    CAN_TxHeaderTypeDef header = {0};
    uint8_t data[8] = {status};
    uint32_t mailbox;

    header.StdId = 0x8EU;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = 1U;

    return HAL_CAN_AddTxMessage(hcan_, &header, data, &mailbox);
}

void HandleEraseFailure(uint32_t failed_sector, uint32_t flash_error) {
    CAN_TxHeaderTypeDef TxHeader = {0};
    uint8_t TxData[8] = {0};
    uint32_t TxMailbox;

    // 0x8F: FC, 0x90: LVC, 0x91: TMS --- ID's
    TxHeader.StdId = 0x8F;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 5;  // Sending 5 bytes of diagnostic data

    TxData[0] = 0xFF;  // Fatal Failure Status
    TxData[1] = (uint8_t)failed_sector;
    TxData[2] = (uint8_t)((flash_error >> 16) & 0xFF);
    TxData[3] = (uint8_t)((flash_error >> 8) & 0xFF);
    TxData[4] = (uint8_t)(flash_error & 0xFF);

    HAL_FLASH_Lock();

    if (HAL_CAN_AddTxMessage(hcan_, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        // Could not queue the failure report.
        Error_Handler();
    }

    // Stay in the bootloader until reset.
    while (1) {
        // we can reset here or implement a watchdog
    }
}

HAL_StatusTypeDef EraseFlash() {
    FLASH_EraseInitTypeDef erase = {0};

    uint32_t sector_error = 0xFFFFFFFFU;

    HAL_StatusTypeDef status = HAL_FLASH_Unlock();

    if (status != HAL_OK) {
        return status;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_ERSERR);

    // Single-bank configuration: erase application sectors 2?11.
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = FLASH_BANK_1;
    erase.Sector = FLASH_SECTOR_2;
    erase.NbSectors = 10;  // Erase sectors 2 through 11
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    status = HAL_FLASHEx_Erase(&erase, &sector_error);

    uint32_t flash_error = HAL_FLASH_GetError();

    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();

    // this flash erase will take 8-10 seconds
    if (status != HAL_OK) {
        HandleEraseFailure(sector_error, flash_error);
        return status;
    }

    return lock_status;
}

static CRC_HandleTypeDef hcrc;

HAL_StatusTypeDef FirmwareCrcInit(void) {
    __HAL_RCC_CRC_CLK_ENABLE();

    hcrc.Instance = CRC;

    // Default polynomial: 0x04C11DB7.
    // Default initial value: 0xFFFFFFFF.
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;

    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

    return HAL_CRC_Init(&hcrc);
}

// Each wait has its own deadline; unrelated traffic cannot extend it.
static HAL_StatusTypeDef WaitForCanFrame(uint32_t id, uint32_t length,
                                         uint8_t data[8], uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    while ((uint32_t)(HAL_GetTick() - start) < timeout_ms) {
        if (HAL_CAN_GetRxFifoFillLevel(hcan_, CAN_RX_FIFO0) == 0U) {
            continue;
        }
        CAN_RxHeaderTypeDef header;
        if (HAL_CAN_GetRxMessage(hcan_, CAN_RX_FIFO0, &header, data) !=
            HAL_OK) {
            return HAL_ERROR;
        }
        if (header.IDE == CAN_ID_STD && header.RTR == CAN_RTR_DATA &&
            header.StdId == id && header.DLC == length) {
            return HAL_OK;
        }
    }
    return HAL_TIMEOUT;
}

int main(void) {
    HAL_Init();

    SystemClock_Config();
    // FirmwareTransfer firmware;

    if (FirmwareCrcInit() != HAL_OK) {
        Error_Handler();
    }

    CanSetup();

    uint8_t data[8];
    uint8_t selected = 0U;
    uint32_t start = HAL_GetTick();

    // Part 1: Wait 3 seconds for ECU Selection
    while ((uint32_t)(HAL_GetTick() - start) < 3000U) {
        uint32_t elapsed = HAL_GetTick() - start;
        if (elapsed >= 3000U) {
            break;
        }
        HAL_StatusTypeDef status =
            WaitForCanFrame(0x8CU, 1U, data, 3000U - elapsed);
        if (status == HAL_TIMEOUT) {
            break;
        }
        if (status != HAL_OK) {
            Error_Handler();
        }
        if (data[0] == 0U) {  // Front Controller selected.
            selected = 1U;
            if (SendAcknowledgementBit(FLASH_ACK_SELECTED) != HAL_OK) {
                Error_Handler();
            }
            break;
        }
    }

    // Part 2: Independent of the 3 sec window, only go through this if this ECU
    // is selected for CanFlash
    if (selected) {
        if (WaitForCanFrame(0x92U, 8U, data, 2000U) != HAL_OK) {
            Error_Handler();  // No fallthrough to the application on failure.
        }
        uint32_t expected_size =
            ((uint32_t)data[0]) | ((uint32_t)data[1] << 8) |
            ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
        uint32_t expected_crc = ((uint32_t)data[4]) | ((uint32_t)data[5] << 8) |
                                ((uint32_t)data[6] << 16) |
                                ((uint32_t)data[7] << 24);
        if (expected_size < 8U ||
            expected_size > APP_FLASH_END - APP_FLASH_START) {
            Error_Handler();
        }

        if (EraseMetadata() != HAL_OK || EraseFlash() != HAL_OK) {
            Error_Handler();
        }
        if (SendAcknowledgementBit(FLASH_ACK_READY) != HAL_OK) {
            Error_Handler();
        }

        uint32_t bytes_written = 0U;
        uint8_t block[256];
        while (bytes_written < expected_size) {
            uint32_t block_bytes = expected_size - bytes_written;
            if (block_bytes > sizeof(block)) {
                block_bytes = sizeof(block);
            }
            if (ReceiveFirmwareBlock(0x8DU, block, block_bytes, 2000U) !=
                    HAL_OK ||
                WriteFirmwareBlock(APP_FLASH_START + bytes_written, block,
                                   block_bytes) != HAL_OK) {
                Error_Handler();
            }
            bytes_written += block_bytes;
            if (SendAcknowledgementBit(FLASH_ACK_BLOCK_WRITTEN) != HAL_OK) {
                Error_Handler();
            }
        }

        uint32_t actual_crc =
            HAL_CRC_Calculate(&hcrc, (uint32_t*)APP_FLASH_START,
                              expected_size) ^
            0xFFFFFFFFU;
        if (actual_crc != expected_crc ||
            SaveMetadata(expected_size, expected_crc) != HAL_OK) {
            Error_Handler();
        }
    }

    // 1. We are ready to jump. Tear down the HAL.
    HAL_DeInit();

    // 2. Disable SysTick timer so it doesn't fire during the jump
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // 3. Extract the Application's Stack Pointer and Reset Handler
    uint32_t app_stack_pointer = *(__IO uint32_t*)APP_ADDRESS;
    uint32_t app_entry_address = *(__IO uint32_t*)(APP_ADDRESS + 4);
    void (*app_entry)(void) = (void (*)(void))app_entry_address;

    // 4. Set the Main Stack Pointer
    __set_MSP(app_stack_pointer);

    // 5. Execute the Jump!
    app_entry();

    /* USER CODE END 2 */

    /* Infinite loop (Should never be reached) */
    while (1) {
    }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified
     * parameters in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
     */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void) {
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x0;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to
 * increment a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM6) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return
     * state
     */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
