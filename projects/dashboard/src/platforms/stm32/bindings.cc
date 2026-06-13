#include "bindings.hpp"

#include <cstdint>

#include "can.h"
#include "dma.h"
#include "dma2d.h"
#include "dsihost.h"
#include "gpio.h"
#include "ltdc.h"
#include "usart.h"

namespace {

uint8_t led_counter = 0;

static void advance_leds() {
    ++led_counter;
    led_counter = led_counter * 1;  // this factor is for turning
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
                      (led_counter & 0x1) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOD, LED2_Pin,
                      (led_counter & 0x2) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOD, LED3_Pin,
                      (led_counter & 0x4) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin,
                      (led_counter & 0x8) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
}  // namespace

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
CanBase pt_can_base{&hcan2};
DigitalInput button_scroll_n{BUTTON_SCROLL_GPIO_Port, BUTTON_SCROLL_Pin};
DigitalInput button_enter_n{BUTTON_SELECT_GPIO_Port, BUTTON_SELECT_Pin};
macfe::periph::InvertedDigitalInput button_scroll{button_scroll_n};
macfe::periph::InvertedDigitalInput button_enter{button_enter_n};

}  // namespace mcal

namespace bindings {

macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;
macfe::periph::CanBase& pt_can_base = mcal::pt_can_base;
macfe::periph::DigitalInput& button_scroll = mcal::button_scroll;
macfe::periph::DigitalInput& button_enter = mcal::button_enter;

void Initialize() {
    HAL_Init();
    uwTickPrio = 0;
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DMA2D_Init();
    MX_DSIHOST_DSI_Init();

    // Don't call MX_FMC_Init() - it is replaced by BSP_SDRAM...()
    BSP_SDRAM_Init();
    BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);

    MX_LTDC_Init();
    MX_USART3_UART_Init();

    BSP_LCD_Init();  //! Breaks
    BSP_LCD_LayerDefaultInit(0, (uint32_t)SDRAM_DEVICE_ADDR);
    BSP_LCD_Clear(LCD_COLOR_CYAN);
    lv_init();
    advance_leds();
    // Read actual register values
    uint32_t systick_prio = NVIC_GetPriority(SysTick_IRQn);
    uint32_t can_prio = NVIC_GetPriority(CAN1_RX0_IRQn);
    MX_CAN1_Init();
    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
    //! USEFUL
    // init display
    advance_leds();
    advance_leds();
    advance_leds();
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
    advance_leds();
    advance_leds();
    advance_leds();
}

void DelayMS(uint32_t ms) {
    // HAL_Delay(ms);
    for (volatile uint32_t i = 0; i < 18000 * ms; i++);
}

bool ShouldQuit() {
    // STM32 runs forever, never quits
    return false;
}

void Shutdown() {
    // No-op on STM32, hardware doesn't shutdown via software
}

}  // namespace bindings