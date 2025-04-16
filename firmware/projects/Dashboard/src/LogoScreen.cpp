#include "inc/LogoScreen.hpp"

#include "FE_Logo.cpp"

LogoScreen::LogoScreen() {}

void LogoScreen::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* menu = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(menu);

    LV_IMG_DECLARE(FE_Logo);

    lv_obj_t* btn = lv_btn_create(menu);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* img = lv_img_create(menu);
    lv_img_set_src(img, &FE_Logo);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(btn, btn_event_handler, LV_EVENT_CLICKED, NULL);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(menu);
}

void LogoScreen::btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = State::SELECT_DRIVER;
    }
}