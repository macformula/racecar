#include "../../bindings.hpp"

#include <cstdint>

#include "can.h"
#include "dma.h"
#include "dma2d.h"
#include "dsihost.h"
#include "gpio.h"
#include "ltdc.h"
#include "usart.h"

// firmware includes
#include "mcal/stm32f4/periph/can.hpp"
#include "mcal/stm32f4/periph/gpio.hpp"

extern "C" {
#include "BSP/STM32469I-Discovery/stm32469i_discovery_sdram.h"
#include "screen_driver.h"
extern void SystemClock_Config(void);
}

/// @brief  SDRAM refresh counter (90 MHz SD clock)
/// @brief  defines copied from the STM32F469I-Disco Board support package
#define REFRESH_COUNT ((uint32_t)0x0569)
#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)

namespace mcal {
using namespace stm32f4::periph;

CanBase veh_can_base{&hcan1};
// TODO swap these labels in CubeMX instead of here
DigitalInput button_scroll{BUTTON_SELECT_GPIO_Port, BUTTON_SELECT_Pin};
DigitalInput button_select{BUTTON_SCROLL_GPIO_Port, BUTTON_SCROLL_Pin};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::DigitalInput& button_scroll = mcal::button_scroll;
shared::periph::DigitalInput& button_select = mcal::button_select;

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

    screen_driver_init();

    mcal::veh_can_base.Setup();
}

void DelayMS(uint32_t ms) {
    HAL_Delay(ms);
}

}  // namespace bindings