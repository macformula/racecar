#include "DashboardFSM.hpp"
#include "Menu.hpp"
#include "lvgl/lvgl.h"

class StartHV : public Menu {
public:
    StartHV();
    static void create_menu();

    static int start_HV_toggle;

private:
    static void start_hv_btn_handler(lv_event_t* e);
};
