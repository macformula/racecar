#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DashboardFSM.hpp"
#include "inc/DashboardMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/LogoScreen.hpp"
#include "inc/ModeSelect.hpp"
#include "inc/StartDriving.hpp"
#include "inc/StartHV.hpp"
#include "inc/StartMotors.hpp"
#include "inc/WaitingScreen.hpp"
#include "lvgl.h"

extern "C" {
extern lv_disp_drv_t lv_display_driver;
}

using namespace generated::can;
VehBus veh_can{bindings::veh_can_base};

int main(void) {
    bindings::Initialize();

    lv_init();
    bindings::PostLvglInit();

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

    LogoScreen logo_screen;
    DashboardMenu dashboard_menu;
    DriveModeMenu drive_menu;
    DriverSelect driver_select;
    ModeSelect modes_select;
    ConfirmMenu confirm_menu;
    WaitingScreen waiting_screen;
    StartHV start_hv;
    StartMotors start_motors;
    StartDriving start_driving;

    dashboard_menu.dashboard_state = STATE_LOGO_WAITING;
    // driver_select.create_menu();
    logo_screen.create_menu();
    dashboardStates previous_state = STATE_LOGO_WAITING;
    logo_screen.create_menu();

    // ---------------------------------------------------------------
    // CAN initialization --------------------------------------------
    // ---------------------------------------------------------------

    // ---------------------------------------------------------------
    // Main Loop ------------------- ---------------------------------
    // ---------------------------------------------------------------

    using DashState = TxDashboardIndicatorStatus::dashState_t;
    DashState dash_state_to_fc = DashState::On;

    while (1) {
        bool scroll = bindings::button_scroll.Read();
        bool select_pressed = bindings::button_select.Read();

        veh_can.Send(TxDashboardIndicatorStatus{
            .dash_state = dash_state_to_fc,

            // driver and event variables should already be enums
            .driver_number =
                static_cast<TxDashboardIndicatorStatus::driverNumber_t>(
                    dashboard_menu.selected_driver),
            .event_number =
                static_cast<TxDashboardIndicatorStatus::eventNumber_t>(
                    dashboard_menu.selected_mode),

            .dash_screen = static_cast<uint8_t>(dashboard_menu.dashboard_state),
            .imd_led = scroll,
            .bms_led = select_pressed,
        });

        // LVGL specific delay
        lv_timer_handler();
        bindings::DelayMS(50);

        // get message from FC
        auto msg = veh_can.GetRxFCDashboardStatus();

        veh_can.Send(TxdashboardTest{.dash_state = msg.has_value()});

        // ---------------------------------------------------------------
        // State Machine -------------------------------------------------
        // ---------------------------------------------------------------

        if (dashboard_menu.dashboard_state != previous_state) {
            lv_obj_t* delete_screen = lv_scr_act();
            previous_state = dashboard_menu.dashboard_state;

            // clang-format off
            switch (dashboard_menu.dashboard_state) {
                case STATE_LOGO_WAITING: logo_screen.create_menu(); break;
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
                    dash_state_to_fc = DashState::RequestedHV;

                    if (msg.has_value() && msg->hvStarted()) {
                        dashboard_menu.dashboard_state = STATE_MOTORS;
                        start_hv.start_HV_toggle = 2;
                    }
                }

                if (start_motors.start_motors_toggle == 1) {
                    dash_state_to_fc = DashState::RequestedMotor;

                    if (msg.has_value() && msg->motorStarted()) {
                        dashboard_menu.dashboard_state = STATE_START_DRIVING;
                        start_motors.start_motors_toggle = 2;
                    }
                }

                break;

            case STATE_START_DRIVING:
                if (msg.has_value() && msg->driveStarted()) {
                    dashboard_menu.dashboard_state = STATE_DRIVE_MODE;
                }

                break;

            default:

                break;
        }
    }

    return 0;
}
