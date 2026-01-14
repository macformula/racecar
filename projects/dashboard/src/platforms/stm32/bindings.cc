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

// drivers
#include "hal_stm_lvgl/screen_driver.h"
#include "lvgl.h"
#include "stm32469i_discovery_lcd.h"
#include "stm32469i_discovery_sdram.h"

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

    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, (uint32_t)SDRAM_DEVICE_ADDR);
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    lv_init();

    // init display
    uint32_t ltdc_layer_index = 0; /* typically 0 or 1 */
#if 0
  // note: direct mode with the LV_USE_DRAW_DMA2D enabled results in glitches on the screen
  void *framebuffer1_address = (void *)SDRAM_DEVICE_ADDR;
  void *framebuffer2_address = (void *)(SDRAM_DEVICE_ADDR + 3 * 1024 * 1024 / 2);
  lv_st_ltdc_create_direct(framebuffer1_address, framebuffer2_address, ltdc_layer_index);
#else
// note: partial mode works fine with the LV_USE_DRAW_DMA2D enabled
#define BUF_SIZE 800 * 48 * 4
    static uint8_t partial_buf1[BUF_SIZE];
    // static uint8_t optional_partial_buf2[BUF_SIZE];
    create_disp(partial_buf1, 0 /*optional_partial_buf2*/, BUF_SIZE,
                ltdc_layer_index);
#endif

    // screen_driver_init();
}

void DelayMS(uint32_t ms) {
    HAL_Delay(ms);
}

bool ShouldQuit() {
    // STM32 runs forever, never quits
    return false;
}

void Shutdown() {
    // No-op on STM32, hardware doesn't shutdown via software
}

}  // namespace bindings