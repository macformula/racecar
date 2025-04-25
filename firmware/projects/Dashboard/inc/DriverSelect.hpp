#pragma once

#include "Menu.hpp"
#include "inc/ButtonHandler.hpp"

class DriverSelect : public Menu {
public:
    DriverSelect();
    static void create_menu();

    static void Update(Button select, Button scroll);

private:
    static lv_obj_t* roller;
};

const uint8_t kNumDrivers = static_cast<uint8_t>(Menu::Driver::UNSPECIFIED);
const char* GetDriverName(Menu::Driver d);