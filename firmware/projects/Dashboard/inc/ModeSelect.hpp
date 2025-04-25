#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class ModeSelect : public Menu {
public:
    ModeSelect();
    static void create_menu();

private:
    static lv_obj_t*
        modes_roller;  // create the roller object that stores the modes names

    static void confirm_btn_event_handler(lv_event_t* e);
    static void up_btn_event_handler(lv_event_t* e);
    static void down_btn_event_handler(lv_event_t* e);
};

const uint8_t kNumModes = static_cast<uint8_t>(Menu::Event::UNSPECIFIED);
const char* GetEventName(Menu::Event e);