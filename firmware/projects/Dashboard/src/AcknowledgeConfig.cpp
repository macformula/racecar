#include "inc/AcknowledgeConfig.hpp"

#include "generated/can/veh_bus.hpp"
#include "inc/Display.hpp"
#include "lvgl.h"

AcknowledgeConfig::AcknowledgeConfig(Display* display) : Screen(display) {}

void AcknowledgeConfig::CreateGUI() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label,
                      "Waiting for response \nfrom FrontController...");
    lv_obj_center(title_label);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
}

void AcknowledgeConfig::Update() {
    auto fc_msg = display_->veh_bus.GetRxFCDashboardStatus();

    if (fc_msg.has_value() && fc_msg->receiveConfig()) {
        display_->ChangeState(State::PRESS_FOR_HV);
    }
}