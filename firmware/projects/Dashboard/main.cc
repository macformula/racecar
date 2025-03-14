#include "bindings.hpp"
#include "lvgl.h"
#include "inc/ButtonHandler.hpp"

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

    // ---------------------------------------------------------------
    // LVGL initialization -------------------------------------------
    // ---------------------------------------------------------------

    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);

    ButtonHandler button_handler(50);

    static lv_indev_drv_t indev_drv_btn;
    lv_indev_drv_init(&indev_drv_btn);
    indev_drv_btn.type = LV_INDEV_TYPE_KEYPAD;

    static ButtonHandler* btn_handler_ptr = &button_handler;
    indev_drv_btn.read_cb = [](lv_indev_drv_t* indev_drv,
                               lv_indev_data_t* data) {
        btn_handler_ptr->update(indev_drv, data);
    };

    lv_indev_t* btn_indev = lv_indev_drv_register(&indev_drv_btn);
    lv_indev_set_group(btn_indev, g);

    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    DashboardMenu dashboard_menu;
    DriveModeMenu drive_menu;
    DriverSelect driver_select;
    ModeSelect modes_select;
    ConfirmMenu confirm_menu;
    WaitingScreen waiting_screen;
    StartHV start_hv;
    StartMotors start_motors;
    StartDriving start_driving;

    dashboard_menu.dashboard_state = STATE_DRIVER;
    driver_select.create_menu();
    dashboardStates previous_state = STATE_DRIVER;

    bindings::DelayMS(5000);

    // ---------------------------------------------------------------
    // CAN initialization --------------------------------------------
    // ---------------------------------------------------------------

    auto msg = veh_can.GetRxFCDashboardStatus();
    bool dash_status = false;
    uint8_t driver_number = 0;
    uint8_t event_number = 0;
    bool start_hv_indicator = false;
    bool start_motor_indicator = false;

    int current_time = lv_tick_get();

    // ---------------------------------------------------------------
    // Main Loop ------------------- ---------------------------------
    // ---------------------------------------------------------------

    while (1) {
        veh_can.Send(TxDashStatus{.status = 1});

        dash_status = true;
        driver_number = static_cast<uint8_t>(dashboard_menu.selected_driver);
        event_number = static_cast<uint8_t>(dashboard_menu.selected_mode);

        //get message from FC
        msg = veh_can.GetRxFCDashboardStatus();

        veh_can.Send(TxDashboardIndicatorStatus{
            .dash_status = dash_status,
            .driver_number = driver_number,
            .event_number = event_number,
            .start_hv_indicator = start_hv_indicator, 
            .start_motor_indicator = start_motor_indicator,
        });

        //LVGL specific delay
        lv_timer_handler();
        bindings::DelayMS(5);

        if(lv_tick_get() > current_time + 10000){
            dashboard_menu.dashboard_state = STATE_HV;
            confirm_menu.initiate_start = 1;
        }

        if(lv_tick_get() > current_time + 20000){
            dashboard_menu.dashboard_state = STATE_WAITING;
            start_hv_indicator = true;
        }






        // ---------------------------------------------------------------
        // State Machine -------------------------------------------------
        // ---------------------------------------------------------------

        if (dashboard_menu.dashboard_state != previous_state) {

            lv_obj_t* delete_screen = lv_scr_act();
            previous_state = dashboard_menu.dashboard_state;

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


            lv_obj_del(delete_screen);
        }

        // ---------------------------------------------------------------
        // CAN receiving processing code ---------------------------------
        // ---------------------------------------------------------------

        switch (dashboard_menu.dashboard_state) {

            case STATE_WAITING:
                if (confirm_menu.initiate_start == 1) {
                    dashboard_menu.dashboard_state = STATE_HV;
                    confirm_menu.initiate_start = 2; 
                }
        
                if (start_hv.start_HV_toggle == 1) {
                    start_hv_indicator = true;
        
                    if (msg->hvStarted()) {
                        dashboard_menu.dashboard_state = STATE_MOTORS;
                        start_hv.start_HV_toggle = 2; 
                    }
                }
        
                if (start_motors.start_motors_toggle == 1) {
                    start_motor_indicator = true;
        
                    if (msg->motorStarted()) {
                        dashboard_menu.dashboard_state = STATE_START_DRIVING;
                        start_motors.start_motors_toggle = 2;
                    }
                }
        
                break;
        
            case STATE_START_DRIVING:
                if (msg->driveStarted()) {
                    dashboard_menu.dashboard_state = STATE_DRIVE_MODE;
                }

                break;
        
            default:

            break;
        }

    }

    return 0;
}