/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/

extern "C" {
    #include <SDL2/SDL.h>
    #include "lvgl/lvgl.h"
    #include "lv_drivers/display/monitor.h"
    #include "lv_drivers/indev/mouse.h"
    #include "lv_drivers/indev/keyboard.h"
    #include "lv_drivers/indev/mousewheel.h"
}



#include "inc/DashboardMenu.hpp"
#include "inc/DebugMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/ProfilesMenu.hpp"
#include "inc/DashboardFSM.hpp"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
extern "C" {
    static void hal_init(void);
    static int tick_thread(void *data);
}

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/









int main(int argc, char **argv)
{
    (void)argc; /*Unused*/
    (void)argv; /*Unused*/

    /* Initialize LVGL */
    lv_init();

    /* Initialize the HAL (display, input devices, tick) for LVGL */
    hal_init();

    // Set up the dashboard
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);
    
    DashboardMenu dashboard_menu;
    DebugMenu debug_menu;
    DriveModeMenu drive_menu;
    ProfilesMenu profiles_menu;

    //initialize the program to start with the main dashboard menu
    dashboard_menu.create_menu();

    //create previous state tracker and initialize to dashboard menu 
    dashboardStates previous_state =  STATE_DASHBOARD;

    while(1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too. */
        lv_timer_handler();
        usleep(5 * 1000);  // Sleep for 5ms

        if (dashboard_menu.dashboard_state != previous_state) {

        // State changed, update menu
        previous_state = dashboard_menu.dashboard_state;

        if (dashboard_menu.dashboard_state == STATE_DASHBOARD) {
            dashboard_menu.create_menu();
        } 
        
        else if (dashboard_menu.dashboard_state == STATE_DRIVE_MODE) {
            drive_menu.create_menu();
        } 
        
        else if (dashboard_menu.dashboard_state == STATE_DEBUG) {
            debug_menu.create_menu();
        } 
        
        else if (dashboard_menu.dashboard_state == STATE_PROFILES) {
            profiles_menu.create_menu();
        }
    }



    }

    return 0;
}










/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
    /* Use the 'monitor' driver which creates a window on PC's monitor to simulate a display */
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

    lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

    lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, th);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    /* Add the mouse as input device */
    mouse_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);  // Basic initialization
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    indev_drv_1.read_cb = mouse_read;
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv_1);

    keyboard_init();
    static lv_indev_drv_t indev_drv_2;
    lv_indev_drv_init(&indev_drv_2);  // Basic initialization
    indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_2.read_cb = keyboard_read;
    lv_indev_t * kb_indev = lv_indev_drv_register(&indev_drv_2);
    lv_indev_set_group(kb_indev, g);
    mousewheel_init();
    static lv_indev_drv_t indev_drv_3;
    lv_indev_drv_init(&indev_drv_3);  // Basic initialization
    indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_3.read_cb = mousewheel_read;

    lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
    lv_indev_set_group(enc_indev, g);

   
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data) {
    (void)data;

    while(1) {
        SDL_Delay(5);
        lv_tick_inc(5);  // Tell LVGL that 5 milliseconds have elapsed
    }

    return 0;
}
