#include "LogoScreen.hpp"

#include "Display.hpp"
#include "FE_Logo.cpp"  // don't open this file! it's big and will crash your IDE

LogoScreen::LogoScreen(Display* display) : Screen(display) {}

void LogoScreen::CreateGUI() {
    LV_IMG_DECLARE(FE_Logo);

    lv_obj_t* img = lv_img_create(frame_);
    lv_img_set_src(img, &FE_Logo);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* help_msg = lv_label_create(frame_);
    lv_label_set_text(help_msg, "Press any button to continue");
    lv_obj_set_style_text_font(help_msg, &lv_font_montserrat_24, 0);
    lv_obj_align(help_msg, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void LogoScreen::Update() {
    if (display_->enter.PosEdge() || display_->scroll.PosEdge()) {
        display_->ChangeState(State::SELECT_PROFILE);
    }
}