#include "inc/WaitingScreen.hpp"

#include "generated/can/veh_bus.hpp"
#include "inc/ButtonHandler.hpp"

WaitingScreen::WaitingScreen() {}

void WaitingScreen::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* confirm_menu = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(confirm_menu);

    // title label
    lv_obj_t* title_label = lv_label_create(confirm_menu);
    lv_label_set_text(title_label,
                      "Waiting for response \nfrom FrontController...");
    lv_obj_center(title_label);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(confirm_menu);
}

void WaitingScreen::Update(Button select, Button scroll) {
    auto fc_msg = Menu::veh_bus.GetRxFCDashboardStatus();
    switch (dashboard_state) {
        case State::WAIT_SELECTION_ACK:
            if (fc_msg.has_value() && fc_msg->receiveConfig()) {
                dashboard_state = State::PRESS_FOR_HV;
            }
            break;

        case State::STARTING_HV:
            if (fc_msg.has_value() && fc_msg->hvStarted()) {
                Menu::dashboard_state = State::PRESS_FOR_MOTOR;
            }
            break;

        case State::STARTING_MOTOR:
            if (fc_msg.has_value() && fc_msg->motorStarted()) {
                dashboard_state = State::BRAKE_TO_START;
            }
            break;

        default:
            break;
    }
    // no button
}