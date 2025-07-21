#include "StartMotors.hpp"

#include "Display.hpp"
#include "lvgl.h"

StartMotors::StartMotors(Display* display) : Screen(display) {}

void StartMotors::CreateGUI() {
    // title label

    static const std::string text[3] = {"Press", "ENTER and BRAKES",
                                        "to start driving"};

    for (int i = 0; i < 3; i++) {
        lv_obj_t* label = lv_label_create(frame_);
        lv_label_set_text(label, text[i].c_str());
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 80 * (i - 1));
        lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    }
}

void StartMotors::Update() {
    if (display_->enter.PosEdge()) {
        display_->ChangeState(State::STARTING_MOTORS);
    }
}