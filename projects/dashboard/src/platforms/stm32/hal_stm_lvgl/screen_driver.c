/*
 * screen.c
 *
 *  Created on: Feb 16, 2023
 *      Author: morgan
 */

#include "screen_driver.h"

#include <lvgl.h>
#include <stdio.h>

#include "main.h"
#include "stm32469i_discovery_lcd.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define LCD_SCREEN_WIDTH NT35510_480X800_HEIGHT
#define LCD_SCREEN_HEIGHT NT35510_480X800_WIDTH
#define FRAMEBUFFER_SIZE (uint32_t)LCD_SCREEN_HEIGHT* LCD_SCREEN_WIDTH
#define DMA_XFERS_NEEDED \
    FRAMEBUFFER_SIZE /   \
        2  // We need half as many transfers because the buffer is an array of
           // 16 bits but the transfers are 32 bits.

/*
 * Handles to peripherals
 */
extern DSI_HandleTypeDef hdsi;  // From main.c
extern LTDC_HandleTypeDef hltdc;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern DMA2D_HandleTypeDef hdma2d;
/*
 * Global variables
 */
lv_display_t* lv_display_driver;
__attribute__((
    section(".framebuffer"))) lv_color_t framebuffer_1[FRAMEBUFFER_SIZE];
__attribute__((
    section(".framebuffer"))) lv_color_t framebuffer_2[FRAMEBUFFER_SIZE];

/*
 * Private functions prototypes
 */
void stm32_flush_cb(lv_display_t* disp_drv, const lv_area_t* area,
                    uint8_t* color_p);
void dma2d_copy_area(lv_area_t area, uint32_t src_buffer, uint32_t dst_buffer);

/*
 * Public functions definitions
 */

void screen_driver_init() {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, (uint32_t)framebuffer_1);

    // LVGL 9.3 display API
    lv_display_driver =
        lv_display_create(NT35510_480X800_HEIGHT, NT35510_480X800_WIDTH);
    lv_display_set_flush_cb(lv_display_driver, stm32_flush_cb);
    lv_display_set_buffers(lv_display_driver, framebuffer_1, framebuffer_2,
                           FRAMEBUFFER_SIZE * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_DIRECT);
}

/*
 * Private functions definitions
 */
void stm32_flush_cb(lv_display_t* disp, const lv_area_t* area,
                    uint8_t* px_map) {
    lv_color_t* color_p = (lv_color_t*)px_map;
    uint16_t *dma_xfer_src, *dma_xfer_dst;

    // Swap the buffer for the one to display and reload the screen at the next
    // vertical blanking
    HAL_LTDC_SetAddress_NoReload(&hltdc, (uint32_t)color_p, 0);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);  // VSYNC

    // Determine source and destination of transfer
    dma_xfer_src = (uint16_t*)color_p;
    if (color_p == framebuffer_1) {
        dma_xfer_dst = (uint16_t*)framebuffer_2;
    } else {
        dma_xfer_dst = (uint16_t*)framebuffer_1;
    }

    // Copy the dirty area (in LVGL 9.3, area is already provided)
    dma2d_copy_area(*area, (uint32_t)dma_xfer_src, (uint32_t)dma_xfer_dst);

    lv_display_flush_ready(disp);
}

void dma2d_copy_area(lv_area_t area, uint32_t src_buffer, uint32_t dst_buffer) {
    size_t start_offset =
        (LCD_SCREEN_WIDTH * (area.y1) + (area.x1)) *
        2;  // address offset (not pixel offset so it is multiplied by 2)
    size_t area_width = 1 + area.x2 - area.x1;
    size_t area_height = 1 + area.y2 - area.y1;
    size_t in_out_offset = LCD_SCREEN_WIDTH - area_width;

    // Set up DMA2D to transfer parts of picture to part of picture
    hdma2d.Init.Mode = DMA2D_M2M;  // plain memory to memory
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    hdma2d.Init.OutputOffset =
        in_out_offset;  // nb pixels in buffer between end of area line and
                        // start of next area line
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputColorMode = DMA2D_INPUT_RGB565;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputOffset = in_out_offset;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputAlpha = 0;

    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, DMA2D_FOREGROUND_LAYER);
    HAL_DMA2D_Start(&hdma2d, src_buffer + start_offset,
                    dst_buffer + start_offset, area_width,
                    area_height);               // Start transfer
    HAL_DMA2D_PollForTransfer(&hdma2d, 10000);  // Wait for transfer to be over
}
