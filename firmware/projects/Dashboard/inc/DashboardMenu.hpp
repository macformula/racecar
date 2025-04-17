#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class DashboardMenu : public Menu {
public:
    DashboardMenu();
    static void create_menu();

private:
    static void drive_btn_event_handler(lv_event_t* e);
    static void debug_btn_event_handler(lv_event_t* e);
    static void profiles_btn_event_handler(lv_event_t* e);
};
