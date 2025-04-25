#pragma once

#include "Menu.hpp"
#include "lvgl/lvgl.h"

class DriverSelect : public Menu {
public:
    DriverSelect();
    static void create_menu();

private:
    // holds all the driver names, this will be manually changed within
    // DriverSelect.cpp to add/remove drivers
    static lv_obj_t* driver_roller;  // create the roller object that stores the
                                     // drivers names

    static void confirm_btn_event_handler(lv_event_t* e);
    static void up_btn_event_handler(lv_event_t* e);
    static void down_btn_event_handler(lv_event_t* e);
};

const uint8_t kNumDrivers = static_cast<uint8_t>(Menu::Driver::UNSPECIFIED);
const char* GetDriverName(Menu::Driver d);