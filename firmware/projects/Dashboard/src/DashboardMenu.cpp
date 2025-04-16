#include "inc/DashboardMenu.hpp"

DashboardMenu::DashboardMenu() {}

void DashboardMenu::create_menu() {
    lv_obj_t* dashboard_screen = lv_obj_create(NULL);  // Create a new screen
    // Call the base class create_menu for common functionality
    Menu::init_menu(dashboard_screen);

    // drive mode button
    lv_obj_t* drive_mode_btn = lv_btn_create(dashboard_screen);
    lv_obj_set_size(drive_mode_btn, 200, 100);
    lv_obj_align(drive_mode_btn, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* drive_label = lv_label_create(drive_mode_btn);
    lv_label_set_text(drive_label, "Drive Mode");
    lv_obj_center(drive_label);
    lv_obj_add_event_cb(drive_mode_btn, drive_btn_event_handler,
                        LV_EVENT_CLICKED, NULL);

    // profiles mode button
    lv_obj_t* profiles_btn = lv_btn_create(dashboard_screen);
    lv_obj_set_size(profiles_btn, 200, 100);
    lv_obj_align(profiles_btn, LV_ALIGN_CENTER, -250, 0);

    lv_obj_t* profiles_label = lv_label_create(profiles_btn);
    lv_label_set_text(profiles_label, "Select Profile");
    lv_obj_center(profiles_label);
    lv_obj_add_event_cb(profiles_btn, profiles_btn_event_handler,
                        LV_EVENT_CLICKED, NULL);

    // debug mode button
    lv_obj_t* debug_btn = lv_btn_create(dashboard_screen);
    lv_obj_set_size(debug_btn, 200, 100);
    lv_obj_align(debug_btn, LV_ALIGN_CENTER, +250, 0);

    lv_obj_t* debug_label = lv_label_create(debug_btn);
    lv_label_set_text(debug_label, "Debug Mode");
    lv_obj_center(debug_label);
    lv_obj_add_event_cb(debug_btn, debug_btn_event_handler, LV_EVENT_CLICKED,
                        NULL);

    // Create title label
    lv_obj_t* title_label = lv_label_create(dashboard_screen);
    lv_label_set_text(title_label, "Formula Electric Dashboard");
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);

    // NOTE: need to run clean for focus to work!
    lv_obj_clean(lv_scr_act());
    lv_scr_load(dashboard_screen);
}

void DashboardMenu::drive_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = State::RUNNING;
    }
}

void DashboardMenu::debug_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        // Menu::dashboard_state = State::debug;
    }
}

void DashboardMenu::profiles_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        // Menu::dashboard_state = STATE_PROFILES;
    }
}
