#include "inc/StartingMotors.hpp"

#include "inc/Button.hpp"
#include "inc/Display.hpp"

StartingMotors::StartingMotors(Display* display) : Screen(display) {}

void StartingMotors::CreateGUI() {
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Starting Motors");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
}

void StartingMotors::Update() {
    auto fc_msg = display_->veh_bus.GetRxFCDashboardStatus();

    if (fc_msg.has_value() && fc_msg->motorStarted()) {
        display_->ChangeState(State::BRAKE_TO_START);
    }
}