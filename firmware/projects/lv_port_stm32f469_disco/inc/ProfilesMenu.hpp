#include "DashboardFSM.hpp"
#include "Menu.hpp"
#include "lvgl.h"

class ProfilesMenu : public Menu {
public:
    ProfilesMenu();
    static void create_menu();

private:
    static void back_btn_event_handler(lv_event_t* e);
};
