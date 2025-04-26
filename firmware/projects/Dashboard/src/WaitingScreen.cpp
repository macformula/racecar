#include "inc/WaitingScreen.hpp"

#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"
#include "inc/Menu.hpp"

WaitingScreen::WaitingScreen(Menu* menu) : Screen(menu) {}

void WaitingScreen::PostCreate() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame);
    lv_label_set_text(title_label,
                      "Waiting for response \nfrom FrontController...");
    lv_obj_center(title_label);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame);
}

void WaitingScreen::Update(Button select, Button scroll) {
    auto fc_msg = menu_->veh_bus.GetRxFCDashboardStatus();
    switch (menu_->state_) {
        case State::WAIT_SELECTION_ACK:
            if (fc_msg.has_value() && fc_msg->receiveConfig()) {
                menu_->ChangeState(State::PRESS_FOR_HV);
            }
            break;

        case State::STARTING_HV:
            if (fc_msg.has_value() && fc_msg->hvStarted()) {
                menu_->ChangeState(State::PRESS_FOR_MOTOR);
            }
            break;

        case State::STARTING_MOTOR:
            if (fc_msg.has_value() && fc_msg->motorStarted()) {
                menu_->ChangeState(State::BRAKE_TO_START);
            }
            break;

        default:
            break;
    }
    // no button
}