#include "inc/Tuning.hpp"

#include <src/core/lv_obj_style.h>
#include <src/font/lv_font.h>
#include <src/misc/lv_area.h>
#include <src/widgets/lv_label.h>

#include "inc/Button.hpp"
#include "inc/Display.hpp"

Tuning::Tuning(Display* display) : Screen(display) {}

void Tuning::CreateGUI() {
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Listening for message");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    lv_obj_t* confirm_label = lv_label_create(frame_);
    lv_label_set_text(confirm_label, "Press any button to confirm selection");
    lv_obj_align(confirm_label, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_set_style_text_font(confirm_label, &lv_font_montserrat_38, 0);
}

void Tuning::Update() {
    if (display_->enter.PosEdge() || display_->scroll.PosEdge()) {
        display_->ChangeState(State::TUNING);
    }
}