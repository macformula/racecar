#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class LogoScreen : public Menu {
public:
    LogoScreen();
    static void create_menu();

private:
    static void btn_event_handler(lv_event_t* e);
};
