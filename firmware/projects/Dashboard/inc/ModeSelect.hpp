#include "Menu.hpp"
#include "lvgl/lvgl.h"
#include "DashboardFSM.hpp"



class ModeSelect : public Menu {
public:
ModeSelect();
    static void create_menu();

private:

    //holds all the mode names, this will be manually changed within ModeSelect.cpp to add/remove modes
    static constexpr int num_modes = 6;
    static const char* modes[num_modes];

    static lv_obj_t* modes_roller; //create the roller object that stores the modes names

    static void confirm_btn_event_handler(lv_event_t* e);
    static void up_btn_event_handler(lv_event_t* e);
    static void down_btn_event_handler(lv_event_t* e);
    
};

