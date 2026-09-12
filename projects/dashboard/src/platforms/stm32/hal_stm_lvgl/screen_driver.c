#include "screen_driver.h"

#include <lvgl_private.h>

#include "ltdc.h"

static void flush_cb(lv_display_t* disp, const lv_area_t* area,
                     uint8_t* px_map);

lv_display_t* create_disp(void* buf1, void* buf2, uint32_t buf_size,
                          uint32_t layer_idx) {
    LTDC_LayerCfgTypeDef* layer_cfg = &hltdc.LayerCfg[layer_idx];
    uint32_t layer_width = layer_cfg->ImageWidth;
    uint32_t layer_height = layer_cfg->ImageHeight;
    lv_color_format_t cf = LV_COLOR_FORMAT_ARGB8888;

    lv_display_t* disp = lv_display_create(layer_width, layer_height);
    lv_display_set_color_format(disp, cf);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_driver_data(disp, (void*)(uintptr_t)layer_idx);

    lv_display_set_buffers(disp, buf1, buf2, buf_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    return disp;
}

static void flush_cb(lv_display_t* disp, const lv_area_t* area,
                     uint8_t* px_map) {
    uint32_t layer_idx = (uint32_t)(uintptr_t)lv_display_get_driver_data(disp);
    LTDC_LayerCfgTypeDef* layer_cfg = &hltdc.LayerCfg[layer_idx];

    int32_t disp_width = disp->hor_res;
    int32_t area_width = area->x2 - area->x1 + 1;
    int32_t area_height = area->y2 - area->y1 + 1;

    uint32_t* fb_p = (uint32_t*)layer_cfg->FBStartAdress +
                     (area->y1 * disp_width + area->x1);
    uint32_t* px_map_p = (uint32_t*)px_map;

    if (area_width == disp_width) {
        lv_memcpy(fb_p, px_map_p, area_width * area_height * 4);
    } else {
        for (int i = 0; i < area_height; i++) {
            for (int j = 0; j < area_width; j++) {
                fb_p[j] = px_map_p[j];
            }
            fb_p += disp_width;
            px_map_p += area_width;
        }
    }

    lv_display_flush_ready(disp);
}
