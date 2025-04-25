#include "inc/LogoScreen.hpp"

#include "FE_Logo.cpp"
#include "generated/can/veh_bus.hpp"

void LogoScreen::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* menu = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(menu);

    LV_IMG_DECLARE(FE_Logo);

    lv_obj_t* img = lv_img_create(menu);
    lv_img_set_src(img, &FE_Logo);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(menu);
}

void LogoScreen::Update(Button select, Button scroll) {
    if (select.IsPressed() || scroll.IsPressed()) {
        Menu::dashboard_state = State::SELECT_DRIVER;
    }
}