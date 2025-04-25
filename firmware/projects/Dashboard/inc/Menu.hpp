#pragma once

#include <cerrno>

#include "../generated/can/veh_messages.hpp"
#include "lvgl/lvgl.h"

class Menu {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;
    using Driver = generated::can::TxDashboardStatus::Driver_t;
    using Event = generated::can::TxDashboardStatus::Event_t;
    Menu();

    static State dashboard_state;

    // holds the selected driver and mode that are updated within DriverSelect
    // and ModeSelect respectively
    static Driver selected_driver;
    static Event selected_event;

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);
};
