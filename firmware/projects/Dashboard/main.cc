#include "bindings.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/Menu.hpp"
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

    menu.screen_->Create();

    while (true) {
        int time_ms = lv_tick_get();
        btn_scroll.Update(time_ms);
        btn_select.Update(time_ms);

        menu.screen_->Update(btn_select, btn_scroll);

        veh_can.Send(TxDashboardStatus{
            .dash_state = menu.GetState(),

            .driver = static_cast<Driver>(menu.selected_driver),
            .event = static_cast<Event>(menu.selected_event),

            .imd_led = btn_scroll.IsPressed(),
            .bms_led = btn_select.IsPressed(),
        });

        lv_timer_handler();
        bindings::DelayMS(20);
    }
}
