#ifndef MENU_H   // If MENU_H is not defined
#define MENU_H   // Define MENU_H

#include "lvgl.h"

class Menu {
public:
    Menu();

    static int menuIndex;

    virtual ~Menu();

    static void init_menu(lv_obj_t* frame);

};

#endif  // End of MENU_H guard
