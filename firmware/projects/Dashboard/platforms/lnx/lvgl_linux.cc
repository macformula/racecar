/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" \
                            issue*/

extern "C" {
#include <SDL2/SDL.h>

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/mousewheel.h"
#include "lvgl/lvgl.h"
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
int tick_thread(void* data) {
    (void)data;

    while (1) {
        SDL_Delay(5);
        lv_tick_inc(5);  // Tell LVGL that 5 milliseconds have elapsed
    }

    return 0;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
void hal_init(void) {
    /* Use the 'monitor' driver which creates a window on PC's monitor to
     * simulate a display */
    monitor_init();
    /* Tick init.
     * You have to call 'lv_tick_inc()' periodically to inform LVGL about
     * how much time has elapsed. Create an SDL thread to do this */
    SDL_CreateThread(tick_thread, "tick", NULL);

    /* Create a display buffer */
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
    static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

    /* Create a display */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);  // Basic initialization
    disp_drv.draw_buf = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    disp_drv.antialiasing = 1;

    lv_disp_t* disp = lv_disp_drv_register(&disp_drv);

    lv_theme_t* th = lv_theme_default_init(
        disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, th);

    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);

    /* Add the mouse as input device */
    mouse_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);  // Basic initialization
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    indev_drv_1.read_cb = mouse_read;
    lv_indev_t* mouse_indev = lv_indev_drv_register(&indev_drv_1);

    keyboard_init();
    static lv_indev_drv_t indev_drv_2;
    lv_indev_drv_init(&indev_drv_2);  // Basic initialization
    indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_2.read_cb = keyboard_read;
    lv_indev_t* kb_indev = lv_indev_drv_register(&indev_drv_2);
    lv_indev_set_group(kb_indev, g);
    mousewheel_init();
    static lv_indev_drv_t indev_drv_3;
    lv_indev_drv_init(&indev_drv_3);  // Basic initialization
    indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_3.read_cb = mousewheel_read;

    lv_indev_t* enc_indev = lv_indev_drv_register(&indev_drv_3);
    lv_indev_set_group(enc_indev, g);
}
