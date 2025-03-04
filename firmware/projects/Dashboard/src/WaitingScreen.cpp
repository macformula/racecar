#include "inc/WaitingScreen.hpp"

WaitingScreen::WaitingScreen() {}

void WaitingScreen::create_menu() {

    //calls base class functionality, handles background and frame
    lv_obj_t* confirm_menu = lv_obj_create(NULL); // Create a new screen
    Menu::init_menu(confirm_menu);

    //title label
    lv_obj_t* title_label = lv_label_create(confirm_menu);
    lv_label_set_text(title_label, "Waiting for response \nfrom FrontController...");
    lv_obj_center(title_label);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);


    //cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(confirm_menu);
}
