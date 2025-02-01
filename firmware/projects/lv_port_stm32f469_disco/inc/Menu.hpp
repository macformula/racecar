#ifndef MENU_H
#define MENU_H

#include "DashboardFSM.hpp"
#include "lvgl.h"

class Menu {
public:
    Menu();

    static dashboardStates dashboard_state;

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);
};

#endif