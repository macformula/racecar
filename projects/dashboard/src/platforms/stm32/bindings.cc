#include "bindings.hpp"

#include <cstdint>

#include "can.h"
#include "dma.h"
#include "dma2d.h"
#include "dsihost.h"
#include "gpio.h"
#include "ltdc.h"
#include "usart.h"

// firmware includes
#include "mcal/stm32f/can.hpp"
#include "mcal/stm32f/gpio.hpp"

extern "C" {
#include "hal_stm_lvgl/screen_driver.h"
#include "stm32469i_discovery_sdram.h"
extern void SystemClock_Config(void);
}

/// @brief  SDRAM refresh counter (90 MHz SD clock)
/// @brief  defines copied from the STM32F469I-Disco Board support package
#define REFRESH_COUNT ((uint32_t)0x0569)
#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)

namespace mcal {
using namespace stm32f;

CanBase veh_can_base{&hcan1};
DigitalInput button_scroll_n{BUTTON_SCROLL_GPIO_Port, BUTTON_SCROLL_Pin};
DigitalInput button_enter_n{BUTTON_SELECT_GPIO_Port, BUTTON_SELECT_Pin};
macfe::periph::InvertedDigitalInput button_scroll{button_scroll_n};
macfe::periph::InvertedDigitalInput button_enter{button_enter_n};

}  // namespace mcal

namespace bindings {

macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;
macfe::periph::DigitalInput& button_scroll = mcal::button_scroll;
macfe::periph::DigitalInput& button_enter = mcal::button_enter;

void Initialize() {
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DMA2D_Init();
    MX_DSIHOST_DSI_Init();
    MX_CAN1_Init();

    // Don't call MX_FMC_Init() - it is replaced by BSP_SDRAM...()
    BSP_SDRAM_Init();
    BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);

    MX_LTDC_Init();
    MX_USART3_UART_Init();

    mcal::veh_can_base.Setup();

    screen_driver_init();
}

void DelayMS(uint32_t ms) {
    HAL_Delay(ms);
}

}  // namespace bindings