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

    // // lv_demo_benchmark();
    // lv_obj_t* label1 = lv_label_create(lv_scr_act());

    // // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    // /* USER CODE END 2 */

    // /* Infinite loop */
    // /* USER CODE BEGIN WHILE */
    // char text[2] = {'*', '\0'};
    // lv_label_set_text(label1, text);
    // lv_obj_center(label1);

    // uint8_t counter = 0;
    // int led = true;

    // while (1) {
    //     lv_timer_handler();
    //     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
    //                       static_cast<GPIO_PinState>(led));
    //     led = !led;

    //     text[0] = counter + 48;
    //     counter++;
    //     if (counter >= 10) counter = 0;
    //     lv_label_set_text(label1, text);

    //     bindings::DelayMs(50);
    // }
}
