#include "main.h"
#include "bindings.hpp"
#include "gpio.h"
#include "inc/DashboardFSM.hpp"
#include "inc/DashboardMenu.hpp"
#include "inc/DebugMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/ProfilesMenu.hpp"
#include "lvgl.h"

extern "C" {
extern lv_disp_drv_t lv_display_driver;
}

//scroll and select button (global for now)
int scroll = 0;
int select = 0;

//button debouncing function
bool debounce_button(bool button_state, uint32_t &last_press_time, uint32_t debounce_time) {
    uint32_t current_time = lv_tick_get(); //use lvgl's tick

    //check if pressed and debounce time passed
    if (button_state && (current_time - last_press_time > debounce_time)) {
        last_press_time = current_time; //update time
        return true; 
    }
    return false; 
}

//setup button press for LVGL's interface
void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
    static int prev_select = 0; 
    static int prev_scroll = 0;

    if (scroll == 1 && prev_scroll == 0) {
        data->key = LV_KEY_NEXT;   //simulate tab)
        data->state = LV_INDEV_STATE_PR;
    } 
    else if (select == 1 && prev_select == 0) {
        data->key = LV_KEY_ENTER;  //simulate enter
        data->state = LV_INDEV_STATE_PR;
    } 
    else {
        data->state = LV_INDEV_STATE_REL;  
    }

    prev_scroll = scroll;
    prev_select = select;
}

//############################################################################
//need to move all this platform specific code to bindings in future

int main(void) {
    lv_init();
    bindings::Initialize();

    //create focus group to place all elements
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    static lv_indev_drv_t indev_drv_btn;
    lv_indev_drv_init(&indev_drv_btn);
    indev_drv_btn.type = LV_INDEV_TYPE_KEYPAD;  //treat as keyboard to simulate tab/enter
    indev_drv_btn.read_cb = button_read;
    lv_indev_t * btn_indev = lv_indev_drv_register(&indev_drv_btn);
    lv_indev_set_group(btn_indev, g);  //attach to focus group, so focus works with just one btn

    //setup dashboard
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    DashboardMenu dashboard_menu;
    DebugMenu debug_menu;
    DriveModeMenu drive_menu;
    ProfilesMenu profiles_menu;

    //init menu
    dashboard_menu.create_menu();

    //track previous dashboard menu state
    dashboardStates previous_state = STATE_DASHBOARD;

    //variables for debouncing
    uint32_t last_scroll_press_time = 0;
    uint32_t last_select_press_time = 0;
    const uint32_t debounce_time = 50; //change to modify debounce time

    

    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too. */
        lv_timer_handler();
        bindings::DelayMS(5);

        //scroll button debouncing
        if (debounce_button(bindings::readScroll(), last_scroll_press_time, debounce_time)) {
            scroll = bindings::readScroll();
        }else{
            scroll = 0;
        }

        //select button debouncing
        if (debounce_button(bindings::readSelect(), last_select_press_time, debounce_time)) {
            select = bindings::readSelect();
        } else {
            select = 0;
        }

        //handle menu changes
        if (dashboard_menu.dashboard_state != previous_state) {
            
            //if state is changed, update previous state
            previous_state = dashboard_menu.dashboard_state;

            //store current screen to soon be deleted
            lv_obj_t* delete_screen = lv_scr_act();

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

            //delete the previous screen as it has been over-written
            lv_obj_del(delete_screen);

        }
    }
}