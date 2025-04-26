#include <iostream>
#include <ostream>

#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/EventSelect.hpp"
#include "inc/LogoScreen.hpp"
#include "inc/Menu.hpp"
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
Menu menu{veh_can};

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

    std::cout << "precreate" << std::endl;
    menu.screen_->Create();
    std::cout << "post create" << std::endl;

    while (true) {
        std::cout << "loop" << std::endl;

        int time_ms = lv_tick_get();
        btn_scroll.Update(time_ms);
        btn_select.Update(time_ms);

        veh_can.Send(TxDashboardStatus{
            .dash_state = menu.state_,

            .driver = static_cast<Driver>(menu.selected_driver),
            .event = static_cast<Event>(menu.selected_event),

            .dash_screen = static_cast<uint8_t>(menu.state_),
            .imd_led = btn_scroll.IsPressed(),
            .bms_led = btn_select.IsPressed(),
        });

        //     // LVGL specific delay
        lv_timer_handler();
        bindings::DelayMS(50);

        // // Handle screen transitions
        // if (Menu::state_ != previous_state) {
        //     previous_state = Menu::state_;

        //     lv_obj_t* delete_screen = lv_scr_act();

        //     using enum State;
        //     switch (Menu::state_) {
        //             // clang-format off
        //         case LOGO:                  screen_ = LogoScreen(); break;
        //         case SELECT_DRIVER:         screen_ =
        //         Driver_select.create_menu(); break; case SELECT_EVENT:
        //         screen_ = Modes_select.create_menu(); break; case
        //         CONFIRM_SELECTION:     screen_ = Confirm_menu.create_menu();
        //         break; case WAIT_SELECTION_ACK:    screen_ =
        //         Waiting_screen.create_menu(); break; case PRESS_FOR_HV:
        //         screen_ = Press_for_hv.create_menu(); break; case
        //         STARTING_HV:           screen_ =
        //         Waiting_screen.create_menu(); break; case PRESS_FOR_MOTOR:
        //         screen_ = Press_for_motor.create_menu(); break; case
        //         STARTING_MOTOR:        screen_ =
        //         Waiting_screen.create_menu(); break; case BRAKE_TO_START:
        //         screen_ = Start_driving.create_menu(); break; case RUNNING:
        //         screen_ = Drive_menu.create_menu(); break;
        //         // clang-format on
        //         default:
        //             break;
        //     }
        //     lv_obj_del(delete_screen);
        // }

        menu.screen_->Update(btn_select, btn_scroll);
    }
}
