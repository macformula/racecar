#pragma once

#include "Menu.hpp"
#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"

class StartHV : public Menu {
public:
    StartHV();
    static void create_menu();

    static void Update(Button select, Button scroll);
};
