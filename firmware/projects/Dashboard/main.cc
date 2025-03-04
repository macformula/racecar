#include "bindings.hpp"
#include "lvgl.h"

// button handling and debouncing class
#include "inc/ButtonHandler.hpp"

// dashboard states classes
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DashboardFSM.hpp"
#include "inc/DashboardMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/ModeSelect.hpp"
#include "inc/StartDriving.hpp"
#include "inc/StartHV.hpp"
#include "inc/StartMotors.hpp"
#include "inc/WaitingScreen.hpp"

extern "C" {
extern lv_disp_drv_t lv_display_driver;
}

using namespace generated::can;
VehBus veh_can{bindings::veh_can_base};

int main(void) {
    lv_init();
    bindings::Initialize();

    // focus group to place all elements on
    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);

    // initialize button handler with 50ms debounce
    ButtonHandler button_handler(50);

    // initialize input buttons to simulate keypad (keyboard) tab and enter
    // buttons
    static lv_indev_drv_t indev_drv_btn;
    lv_indev_drv_init(&indev_drv_btn);
    indev_drv_btn.type = LV_INDEV_TYPE_KEYPAD;

    // button handler callback
    static ButtonHandler* btn_handler_ptr = &button_handler;
    indev_drv_btn.read_cb = [](lv_indev_drv_t* indev_drv,
                               lv_indev_data_t* data) {
        btn_handler_ptr->update(indev_drv, data);
    };

    // register inputs
    lv_indev_t* btn_indev = lv_indev_drv_register(&indev_drv_btn);
    lv_indev_set_group(btn_indev, g);

    // setup main program frame
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    // initialize all screens
    DashboardMenu dashboard_menu;
    DriveModeMenu drive_menu;
    DriverSelect driver_select;
    ModeSelect modes_select;
    ConfirmMenu confirm_menu;
    WaitingScreen waiting_screen;
    StartHV start_hv;
    StartMotors start_motors;
    StartDriving start_driving;

    // start with driver select screen
    dashboard_menu.dashboard_state = STATE_DRIVER;
    driver_select.create_menu();
    dashboardStates previous_state = STATE_DRIVER;

    bindings::DelayMS(5000);

    while (1) {
        veh_can.Send(TxDashStatus{.status = 1});

        // lvgl screen update tasks
        lv_timer_handler();
        bindings::DelayMS(5);

        if (dashboard_menu.dashboard_state != previous_state) {
            // store current screen and update it
            lv_obj_t* delete_screen = lv_scr_act();
            previous_state = dashboard_menu.dashboard_state;

            // run screen creation for the corresponding screen
            // clang-format off
            switch (dashboard_menu.dashboard_state) {
                case STATE_DASHBOARD:    dashboard_menu.create_menu(); break;
                case STATE_DRIVER:       driver_select.create_menu(); break;
                case STATE_MODE:         modes_select.create_menu(); break;
                case STATE_CONFIRM:      confirm_menu.create_menu(); break;
                case STATE_WAITING:      waiting_screen.create_menu(); break;
                case STATE_HV:           start_hv.create_menu(); break;
                case STATE_MOTORS:       start_motors.create_menu(); break;
                case STATE_START_DRIVING: start_driving.create_menu(); break;
                case STATE_DRIVE_MODE:   drive_menu.create_menu(); break;
                default: break;
            }
            // clang-format on

            // delete previous screen to save memory
            lv_obj_del(delete_screen);
        }

        // Testing code
        // ---------------------------------------------------------------
        // simulates waiting for CAN confirmation that the vehicle is ready to
        // start HV after confirm button press, wait 2 seconds before allowing
        // HV startup
        static uint32_t start_time = 0;
        if (confirm_menu.initiate_start == 1 &&
            dashboard_menu.dashboard_state == STATE_WAITING) {
            if (start_time == 0) {
                start_time = lv_tick_get();
            }
            if (lv_tick_get() - start_time >= 2000) {
                dashboard_menu.dashboard_state = STATE_HV;
                confirm_menu.initiate_start = 2;
                start_time = 0;
            }
        }

        // simulates waiting for CAN confirmation that HV is fully enabled
        // after HV enabled, wait 2 seconds before allowing motor startup
        static uint32_t start_time1 = 0;
        if (start_hv.start_HV_toggle == 1 &&
            dashboard_menu.dashboard_state == STATE_WAITING) {
            if (start_time1 == 0) {
                start_time1 = lv_tick_get();
            }
            if (lv_tick_get() - start_time1 >= 2000) {
                dashboard_menu.dashboard_state = STATE_MOTORS;
                start_hv.start_HV_toggle = 2;
                start_time1 = 0;
            }
        }

        // simulates waiting for CAN confirmation that motors are enabled and
        // ready after motors enabled, wait 2 seconds before showing
        // ready-to-drive screen
        static uint32_t start_time2 = 0;
        if (start_motors.start_motors_toggle == 1 &&
            dashboard_menu.dashboard_state == STATE_WAITING) {
            if (start_time2 == 0) {
                start_time2 = lv_tick_get();
            }
            if (lv_tick_get() - start_time2 >= 2000) {
                dashboard_menu.dashboard_state = STATE_START_DRIVING;
                start_motors.start_motors_toggle = 2;
                start_time2 = 0;
            }
        }

        // simulates waiting for user to press breaks to start driving
        // shows ready-to-drive message for 3 seconds before switching to drive
        // mode
        static uint32_t start_time3 = 0;
        if (dashboard_menu.dashboard_state == STATE_START_DRIVING) {
            if (start_time3 == 0) {
                start_time3 = lv_tick_get();
            }
            if (lv_tick_get() - start_time3 >= 3000) {
                dashboard_menu.dashboard_state = STATE_DRIVE_MODE;
                start_time3 = 0;
            }
        }
        // Testing code
        // ---------------------------------------------------------------
    }

    return 0;
}