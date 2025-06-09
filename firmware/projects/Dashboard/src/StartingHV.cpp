#include "inc/StartingHV.hpp"

#include "inc/Button.hpp"
#include "inc/Display.hpp"

StartingHV::StartingHV(Display* display) : Screen(display) {}

void StartingHV::CreateGUI() {
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Precharging Accumulator");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    progress_bar_ = lv_bar_create(frame_);
    lv_obj_set_size(progress_bar_, 600, 30);
    lv_obj_center(progress_bar_);
}

void StartingHV::Update() {
    auto fc_msg = display_->veh_bus.GetRxDashCommand();

    if (!fc_msg.has_value()) return;

    lv_bar_set_value(progress_bar_, fc_msg->HvChargePercent(), LV_ANIM_OFF);

    if (fc_msg->HvStarted()) {
        display_->ChangeState(State::PRESS_FOR_MOTOR);
    }
}