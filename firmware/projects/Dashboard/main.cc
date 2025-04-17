#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/ConfirmMenu.hpp"
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
using State = TxDashboardStatus::DashState_t;
using Driver = TxDashboardStatus::Driver_t;
using Event = TxDashboardStatus::Event_t;

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
    DashboardMenu dashboard_menu;  // unused
    DriveModeMenu drive_menu;
    DriverSelect driver_select;
    ModeSelect modes_select;
    ConfirmMenu confirm_menu;
    WaitingScreen waiting_screen;
    StartHV press_for_hv;
    StartMotors press_for_motor;
    StartDriving start_driving;

    dashboard_menu.dashboard_state = State::LOGO;
    State previous_state = dashboard_menu.dashboard_state;
    logo_screen.create_menu();

    while (true) {
        veh_can.Send(TxDashboardStatus{
            .dash_state = dashboard_menu.dashboard_state,

            .driver = static_cast<Driver>(dashboard_menu.selected_driver),
            .event = static_cast<Event>(dashboard_menu.selected_mode),

            .dash_screen = static_cast<uint8_t>(dashboard_menu.dashboard_state),
            .imd_led = button_handler.getScrollState(),
            .bms_led = button_handler.getSelectState(),
        });

        // LVGL specific delay
        lv_timer_handler();
        bindings::DelayMS(50);

        // Handle screen transitions
        if (dashboard_menu.dashboard_state != previous_state) {
            previous_state = dashboard_menu.dashboard_state;

            lv_obj_t* delete_screen = lv_scr_act();

            using enum State;
            switch (dashboard_menu.dashboard_state) {
                    // clang-format off
                case LOGO:                  logo_screen.create_menu();      break;
                case SELECT_DRIVER:         driver_select.create_menu();    break;
                case SELECT_EVENT:          modes_select.create_menu();     break;
                case CONFIRM_SELECTION:     confirm_menu.create_menu();     break;
                case WAIT_SELECTION_ACK:    waiting_screen.create_menu();   break;
                case PRESS_FOR_HV:          press_for_hv.create_menu();     break;
                case STARTING_HV:           waiting_screen.create_menu();   break;
                case PRESS_FOR_MOTOR:       press_for_motor.create_menu();  break;
                case STARTING_MOTOR:        waiting_screen.create_menu();   break;
                case BRAKE_TO_START:        start_driving.create_menu();    break;
                case RUNNING:               drive_menu.create_menu();       break;
                // clang-format on
                default:
                    break;
            }
            lv_obj_del(delete_screen);
        }

        // get message from FC
        auto msg = veh_can.GetRxFCDashboardStatus();

        switch (dashboard_menu.dashboard_state) {
            case State::LOGO:
                break;

            case State::SELECT_DRIVER:
                break;

            case State::SELECT_EVENT:
                break;

            case State::CONFIRM_SELECTION:
                break;

            case State::WAIT_SELECTION_ACK:
                if (msg.has_value() && msg->receiveConfig()) {
                    dashboard_menu.dashboard_state = State::PRESS_FOR_HV;
                }
                break;

            case State::PRESS_FOR_HV:
                break;

            case State::STARTING_HV:
                if (msg.has_value() && msg->hvStarted()) {
                    dashboard_menu.dashboard_state = State::PRESS_FOR_MOTOR;
                }

                break;

            case State::PRESS_FOR_MOTOR:
                break;

            case State::STARTING_MOTOR:
                if (msg.has_value() && msg->motorStarted()) {
                    dashboard_menu.dashboard_state = State::BRAKE_TO_START;
                }

                break;

            case State::BRAKE_TO_START:
                if (msg.has_value() && msg->driveStarted()) {
                    dashboard_menu.dashboard_state = State::RUNNING;
                }

                break;

            case State::RUNNING:
                break;
        }
    }
}