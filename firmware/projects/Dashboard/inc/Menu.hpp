#ifndef MENU_H   
#define MENU_H   

#include "lvgl/lvgl.h"
#include "DashboardFSM.hpp"


class Menu {
public:
    Menu();

    static dashboardStates dashboard_state;

    //holds the selected driver and mode that are updated within DriverSelect and ModeSelect respectively
    static int selected_driver;
    static int selected_mode;

    static int front_controller_status[3];

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);

};


#endif