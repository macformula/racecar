/// @author Manush Patel
/// @date 2026-07

#pragma once
#include "stm32f7xx_hal.h"

// =============================================================================
// Physical Pin Configurations
// =============================================================================
#define CAN1_TX_PORT GPIOA
#define CAN1_TX_PIN GPIO_PIN_12
#define CAN1_RX_PORT GPIOA
#define CAN1_RX_PIN GPIO_PIN_11
#define CAN1_AF GPIO_AF9_CAN1

#define CAN2_TX_PORT GPIOB
#define CAN2_TX_PIN GPIO_PIN_13
#define CAN2_RX_PORT GPIOB
#define CAN2_RX_PIN GPIO_PIN_5
#define CAN2_AF GPIO_AF9_CAN2

// =============================================================================
// Active Hardware Selection (Overrideable thru platformio.ini)
// =============================================================================
#ifndef BOOTLOADER_CAN_INSTANCE
#define BOOTLOADER_CAN_INSTANCE CAN1  // Fallback default
#endif

// =============================================================================
// Memory Boundaries
// =============================================================================
#define RAM_BUFFER_SIZE_BYTES (256 * 1024)  // 256 KB static staging buffer
#define BOOTLOADER_RAM_BUF_SIZE RAM_BUFFER_SIZE_BYTES

// Sector 0 starting location in flash (Where the app starts executing)
#define BOOTLOADER_FLASH_START 0x08000000U
#define FLASH_START 0x08000000U
#define FLASH_SIZE (2U * 1024U * 1024U)       // 2MB, in bytes
#define FLASH_END (FLASH_START + FLASH_SIZE)  // 0x08200000

// =============================================================================
// Network Identity & Timings
// =============================================================================
#ifndef BOOTLOADER_CONFIG_ID
#define BOOTLOADER_CONFIG_ID 0  // FC = 0, LVC = 1, TMS = 2
#endif

#ifndef BOOTLOADER_TIMEOUT_MS
#define BOOTLOADER_TIMEOUT_MS 3000
#endif

#define FLASH_KEY1 0x45670123

#define FLASH_KEY2 0xCDEF89AB