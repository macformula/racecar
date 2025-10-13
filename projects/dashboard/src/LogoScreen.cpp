#include "LogoScreen.hpp"

#include "Display.hpp"

// Include the logo image data (defined in FE_Logo.cpp)
#include "FE_Logo.cpp"  // don't open this file! it's big and will crash your IDE

LogoScreen::LogoScreen(Display* display) : Screen(display) {}

void LogoScreen::CreateGUI() {
    // Create and display the Formula Electric logo
    lv_obj_t* logo = lv_image_create(frame_);
    lv_image_set_src(logo, &FE_Logo);
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -20);

    // Help message at the bottom
    lv_obj_t* help_msg = lv_label_create(frame_);
    lv_label_set_text(help_msg, "Press Tab or Space to continue");
    lv_obj_align(help_msg, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void LogoScreen::Update() {
    if (display_->enter.PosEdge() || display_->scroll.PosEdge()) {
        display_->ChangeState(State::SELECT_PROFILE);
    }
}