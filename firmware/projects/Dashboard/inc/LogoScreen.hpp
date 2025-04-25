#pragma once

#include "Menu.hpp"
#include "inc/ButtonHandler.hpp"

class LogoScreen : public Menu {
public:
    static void create_menu();

    static void Update(Button select, Button scroll);
};
