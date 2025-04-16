#include "DashboardFSM.hpp"
#include "Menu.hpp"
#include "lvgl/lvgl.h"

class WaitingScreen : public Menu {
public:
    WaitingScreen();
    static void create_menu();

private:
};
