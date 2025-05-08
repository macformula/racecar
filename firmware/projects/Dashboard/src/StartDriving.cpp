#include "inc/StartDriving.hpp"

#include "inc/Display.hpp"
#include "lvgl.h"

StartDriving::StartDriving(Display* display) : Screen(display) {}

void StartDriving::CreateGUI() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Press brakes to start driving!");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
}

void StartDriving::Update() {
    auto fc_msg = display_->veh_bus.GetRxFCDashboardStatus();
    if (fc_msg.has_value() && fc_msg->driveStarted()) {
        display_->ChangeState(State::RUNNING);
    }
}
