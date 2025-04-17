#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class ConfirmMenu : public Menu {
public:
    ConfirmMenu();
    static void create_menu();

private:
    static void confirm_btn_event_handler(lv_event_t* e);
    static void restart_btn_event_handler(lv_event_t* e);
};
