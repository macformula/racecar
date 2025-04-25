#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/EventSelect.hpp"
#include "inc/LogoScreen.hpp"
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
VehBus& Menu::veh_bus = veh_can;

Button btn_scroll{bindings::button_scroll};
Button btn_select{bindings::button_select};

int main(void) {
    lv_init();
    bindings::Initialize();

    bindings::PostLvglInit();

    // ---------------------------------------------------------------
    // LVGL initialization -------------------------------------------
    // ---------------------------------------------------------------
    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);

    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    LogoScreen logo_screen;
    DriveModeMenu drive_menu;
    DriverSelect driver_select;
    EventSelect modes_select;
    ConfirmMenu confirm_menu;
    WaitingScreen waiting_screen;
    StartHV press_for_hv;
    StartMotors press_for_motor;
    StartDriving start_driving;

    Menu::dashboard_state = State::LOGO;
    State previous_state = Menu::dashboard_state;
    logo_screen.create_menu();

    while (true) {
        int time_ms = lv_tick_get();
        btn_scroll.Update(time_ms);
        btn_select.Update(time_ms);

        veh_can.Send(TxDashboardStatus{
            .dash_state = Menu::dashboard_state,

            .driver = static_cast<Driver>(Menu::selected_driver),
            .event = static_cast<Event>(Menu::selected_event),

            .dash_screen = static_cast<uint8_t>(Menu::dashboard_state),
            .imd_led = btn_scroll.IsPressed(),
            .bms_led = btn_select.IsPressed(),
        });

        //     // LVGL specific delay
        lv_timer_handler();
        bindings::DelayMS(50);

        // Handle screen transitions
        if (Menu::dashboard_state != previous_state) {
            previous_state = Menu::dashboard_state;

            lv_obj_t* delete_screen = lv_scr_act();

            using enum State;
            switch (Menu::dashboard_state) {
                    // clang-format off
                case LOGO:                  logo_screen.create_menu(); break;
                case SELECT_DRIVER:         driver_select.create_menu(); break;
                case SELECT_EVENT:          modes_select.create_menu(); break;
                case CONFIRM_SELECTION:     confirm_menu.create_menu(); break;
                case WAIT_SELECTION_ACK:    waiting_screen.create_menu(); break;
                case PRESS_FOR_HV:          press_for_hv.create_menu(); break;
                case STARTING_HV:           waiting_screen.create_menu(); break;
                case PRESS_FOR_MOTOR:       press_for_motor.create_menu(); break;
                case STARTING_MOTOR:        waiting_screen.create_menu(); break;
                case BRAKE_TO_START:        start_driving.create_menu(); break;
                case RUNNING:               drive_menu.create_menu(); break;
                // clang-format on
                default:
                    break;
            }
            lv_obj_del(delete_screen);
        }

        switch (Menu::dashboard_state) {
            case State::LOGO:
                logo_screen.Update(btn_select, btn_scroll);
                break;

            case State::SELECT_DRIVER:
                driver_select.Update(btn_select, btn_scroll);
                break;

            case State::SELECT_EVENT:
                modes_select.Update(btn_select, btn_scroll);
                break;

            case State::CONFIRM_SELECTION:
                confirm_menu.Update(btn_select, btn_scroll);
                break;

            case State::WAIT_SELECTION_ACK:
                waiting_screen.Update(btn_select, btn_scroll);
                break;

            case State::PRESS_FOR_HV:
                press_for_hv.Update(btn_select, btn_scroll);
                break;

            case State::STARTING_HV:
                waiting_screen.Update(btn_select, btn_scroll);
                break;

            case State::PRESS_FOR_MOTOR:
                press_for_motor.Update(btn_select, btn_scroll);
                break;

            case State::STARTING_MOTOR:
                waiting_screen.Update(btn_select, btn_scroll);
                break;

            case State::BRAKE_TO_START:
                start_driving.Update(btn_select, btn_scroll);
                break;

            case State::RUNNING:
                break;
        }
    }
}
