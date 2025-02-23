#include "inc/ConfirmMenu.hpp"

int ConfirmMenu::initiate_start = 0;

ConfirmMenu::ConfirmMenu() {}

void ConfirmMenu::create_menu() {

    //calls base class functionality, handles background and frame
    lv_obj_t* confirm_menu = lv_obj_create(NULL); // Create a new screen
    Menu::init_menu(confirm_menu);

    //title label
    lv_obj_t* title_label = lv_label_create(confirm_menu);
    lv_label_set_text(title_label, "Confirm Selection");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    //display selected driver and mode
    lv_obj_t* selection_label = lv_label_create(confirm_menu);
    lv_label_set_text_fmt(selection_label, "Driver: %d | Mode: %d", selected_driver, selected_mode);
    lv_obj_align(selection_label, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(selection_label, &lv_font_montserrat_24, 0);

    //confirm button
    lv_obj_t* confirm_btn = lv_btn_create(confirm_menu);
    lv_obj_set_size(confirm_btn, 100, 50);
    lv_obj_center(confirm_btn);
    lv_obj_set_x(confirm_btn, lv_obj_get_x(confirm_btn) - 150);
    lv_obj_t* confirm_btn_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_btn_label, "Confirm");
    lv_obj_center(confirm_btn_label);
    lv_obj_add_event_cb(confirm_btn, confirm_btn_event_handler, LV_EVENT_CLICKED, NULL);

    //reset button
    lv_obj_t* reset_btn = lv_btn_create(confirm_menu);
    lv_obj_set_size(reset_btn, 100, 50);
    lv_obj_center(reset_btn);
    lv_obj_set_x(reset_btn, lv_obj_get_x(reset_btn) + 150);
    lv_obj_t* reset_btn_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_btn_label, "Reset");
    lv_obj_center(reset_btn_label);
    lv_obj_add_event_cb(reset_btn, restart_btn_event_handler, LV_EVENT_CLICKED, NULL);

    //cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(confirm_menu);
}

//proceeds to next screen
void ConfirmMenu::confirm_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = STATE_WAITING;

        //start initiated, implement logic in main function to detect this and proceed with CAN
        initiate_start = 1;
    }
}

//restarts to driverselect
void ConfirmMenu::restart_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = STATE_DRIVER;
    }
}
