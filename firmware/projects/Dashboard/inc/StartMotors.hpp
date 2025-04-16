#include "DashboardFSM.hpp"
#include "Menu.hpp"
#include "lvgl/lvgl.h"

class StartMotors : public Menu {
public:
    StartMotors();
    static void create_menu();

    static int start_motors_toggle;

private:
    static void start_motors_btn_handler(lv_event_t* e);
};
