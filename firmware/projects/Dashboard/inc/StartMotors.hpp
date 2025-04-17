#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class StartMotors : public Menu {
public:
    StartMotors();
    static void create_menu();

private:
    static void start_motors_btn_handler(lv_event_t* e);
};
