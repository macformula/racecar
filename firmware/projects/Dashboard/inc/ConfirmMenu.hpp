#include "Menu.hpp"
#include "lvgl/lvgl.h"
#include "DashboardFSM.hpp"



class ConfirmMenu : public Menu {
public:
    ConfirmMenu();
    static void create_menu();

    static int initiate_start;

private:

    static void confirm_btn_event_handler(lv_event_t* e);
    static void restart_btn_event_handler(lv_event_t* e);
    
};

