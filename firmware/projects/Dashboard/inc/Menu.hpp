#pragma once

#include "../generated/can/veh_messages.hpp"
#include "lvgl/lvgl.h"

class Menu {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;
    Menu();

    static State dashboard_state;

    // holds the selected driver and mode that are updated within DriverSelect
    // and ModeSelect respectively
    static int selected_driver;
    static int selected_mode;

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);
};
