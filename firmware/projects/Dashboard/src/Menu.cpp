#include "inc/Menu.hpp"

Menu::Menu() {}

Menu::~Menu() {}

dashboardStates Menu::dashboard_state = STATE_DASHBOARD;

int Menu::selected_driver = 0;
int Menu::selected_mode = 0;
int Menu::front_controller_status[3] = {0, 0, 0};

void Menu::init_menu(lv_obj_t* frame) {
    // Create a background for the menu
    lv_obj_t* rect = lv_obj_create(frame);
    lv_obj_set_size(rect, 800 - 20, 480 - 20);  // Adjust size for margins
    lv_obj_align(rect, LV_ALIGN_CENTER, 0, 0);  // Center the rectangle
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xD3D3D3), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, LV_PART_MAIN);  // Fully opaque
}