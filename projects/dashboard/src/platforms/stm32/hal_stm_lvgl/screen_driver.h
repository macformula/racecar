#ifndef SCREEN_DRIVER_H
#define SCREEN_DRIVER_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

lv_display_t* create_disp(void* buf1, void* buf2, uint32_t buf_size,
                          uint32_t layer_idx);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SCREEN_DRIVER_H*/
