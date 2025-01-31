#ifndef MENU_H   
#define MENU_H   

#include "lvgl/lvgl.h"
#include "DashboardFSM.hpp"


class Menu {
public:
    Menu();

    static dashboardStates dashboard_state;

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);

};


#endif