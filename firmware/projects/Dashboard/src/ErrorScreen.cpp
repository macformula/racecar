#include "inc/ErrorScreen.hpp"

#include "generated/can/veh_messages.hpp"
#include "inc/Button.hpp"
#include "inc/Display.hpp"
#include "lvgl.h"

using namespace generated::can;

ErrorScreen::ErrorScreen(Display* display) : Screen(display) {}

void ErrorScreen::CreateGUI(void) {
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Oops! There were some errors!");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    error_text_ = lv_textarea_create(frame_);
    lv_textarea_set_text(error_text_, "Where are the errors??");
    lv_obj_set_width(error_text_, 500);
    lv_obj_align(error_text_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(error_text_, &lv_font_montserrat_24, 0);

    lv_obj_t* footer = lv_label_create(frame_);
    lv_label_set_text(footer, "Hold ENTER to shutdown");
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_24, 0);
}

void ErrorScreen::Update(void) {
    auto fc_msg = display_->veh_bus.GetRxFcAlerts();

    if (display_->enter.GetHeldDuration() > 3000 &&
        display_->enter.IsPressed()) {
        display_->ChangeState(State::SHUTDOWN);
    }

    std::string codes = "Where are the errors??";
    if (fc_msg.has_value()) {
        shared::can::RawMessage alerts = fc_msg->Raw();
        for (size_t idx = 0; idx < alerts.data_length; idx++) {
            for (size_t bit = 0; bit < 8; bit++) {
                if (alerts.data[idx] & (1 << bit)) {
                    codes += std::to_string(idx * 8 + bit);
                    codes += "  ";
                }
            }
        }
    }
    lv_textarea_set_text(error_text_, codes.c_str());
}