#include "Menu.hpp"
#include "lvgl/lvgl.h"
#include "DashboardFSM.hpp"


class DebugMenu : public Menu {
public:
    DebugMenu();
    static void create_menu();

private:
    static void back_btn_event_handler(lv_event_t* e);
};
