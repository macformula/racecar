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
int main(void) {
    lv_init();

    bindings::Initialize();

    // Set up the dashboard
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    DashboardMenu dashboard_menu;
    DebugMenu debug_menu;
    DriveModeMenu drive_menu;
    ProfilesMenu profiles_menu;

    // initialize the program to start with the main dashboard menu
    dashboard_menu.create_menu();

    // create previous state tracker and initialize to dashboard menu
    dashboardStates previous_state = STATE_DASHBOARD;

    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too. */
        lv_timer_handler();
        bindings::DelayMS(5);

        if (dashboard_menu.dashboard_state != previous_state) {
            
            //state changed, update menu
            previous_state = dashboard_menu.dashboard_state;

            //store current screen that needs to be deleted
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

            //delete the previous screen that was overwritten
            lv_obj_del(delete_screen);  

        }
    }

}
