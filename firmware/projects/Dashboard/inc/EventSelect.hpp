#pragma once

#include "Menu.hpp"
#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class EventSelect : public Menu {
public:
    EventSelect();
    static void create_menu();

    void Update(Button select, Button scroll);

private:
    static lv_obj_t*
        roller;  // create the roller object that stores the modes names
};

const uint8_t kEventCount = static_cast<uint8_t>(Menu::Event::UNSPECIFIED);
const char* GetEventName(Menu::Event e);