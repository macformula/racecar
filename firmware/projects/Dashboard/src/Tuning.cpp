#include "inc/Tuning.hpp"

#include "inc/Button.hpp"
#include "inc/Display.hpp"

Tuning::Tuning(Display* display) : Screen(display) {}

void Tuning::CreateGUI() {
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Listening for message");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
}

void Tuning::Update() {
    if (display_->enter.PosEdge() || display_->scroll.PosEdge()) {
        display_->ChangeState(State::SELECT_PROFILE);
    }
}