#pragma once

#include "Menu.hpp"
#include "inc/ButtonHandler.hpp"

class StartDriving : public Menu {
public:
    StartDriving();
    static void create_menu();

    void Update(Button select, Button scroll);

private:
};
