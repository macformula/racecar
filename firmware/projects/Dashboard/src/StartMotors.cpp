#include "inc/StartMotors.hpp"

#include "inc/Display.hpp"
#include "lvgl.h"

StartMotors::StartMotors(Display* display) : Screen(display) {}

void StartMotors::CreateGUI() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Press ENTER to start motors");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
}

void StartMotors::Update() {
    if (display_->enter.PosEdge()) {
        display_->ChangeState(State::STARTING_MOTORS);
    }
}