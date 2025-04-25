#pragma once

#include "Menu.hpp"
#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"

class StartMotors : public Menu {
public:
    StartMotors();
    static void create_menu();

    static void Update(Button select, Button scroll);
};
