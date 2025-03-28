#include "DashboardFSM.hpp"
#include "Menu.hpp"
#include "lvgl/lvgl.h"

class DriverSelect : public Menu {
public:
    DriverSelect();
    static void create_menu();

private:
    // holds all the driver names, this will be manually changed within
    // DriverSelect.cpp to add/remove drivers
    static constexpr int num_drivers = 7;
    static const char* drivers[num_drivers];

    static lv_obj_t* driver_roller;  // create the roller object that stores the
                                     // drivers names

    static void confirm_btn_event_handler(lv_event_t* e);
    static void up_btn_event_handler(lv_event_t* e);
    static void down_btn_event_handler(lv_event_t* e);
};
