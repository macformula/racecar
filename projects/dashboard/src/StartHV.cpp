#include "StartHV.hpp"

#include "Display.hpp"
#include "lvgl.h"

StartHV::StartHV(Display* display) : Screen(display) {}

void StartHV::CreateGUI() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Press ENTER to start HV");
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_48, 0);
}

void StartHV::Update() {
    if (display_->enter.PosEdge()) {
        display_->ChangeState(State::STARTING_HV);
    }
}
