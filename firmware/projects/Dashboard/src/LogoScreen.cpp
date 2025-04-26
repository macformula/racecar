#include "inc/LogoScreen.hpp"

#include "FE_Logo.cpp"
#include "inc/Menu.hpp"

LogoScreen::LogoScreen(Menu* menu) : Screen(menu) {}

void LogoScreen::PostCreate() {
    LV_IMG_DECLARE(FE_Logo);

    lv_obj_t* img = lv_img_create(frame);
    lv_img_set_src(img, &FE_Logo);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame);
}

void LogoScreen::Update(Button select, Button scroll) {
    if (select.IsPressed() || scroll.IsPressed()) {
        menu_->ChangeState(State::SELECT_DRIVER);
    }
}