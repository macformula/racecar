#pragma once

#include "Menu.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class WaitingScreen : public Menu {
public:
    WaitingScreen();
    static void create_menu();

    static void Update(Button select, Button scroll);

private:
};
