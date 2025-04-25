#pragma once

#include "Menu.hpp"
#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class ConfirmMenu : public Menu {
    enum Selection : bool {
        CONFIRM = false,
        RESET = true
    };

public:
    ConfirmMenu();
    static void create_menu();

    static void Update(Button select, Button scroll);

private:
    static lv_obj_t* roller;
};
