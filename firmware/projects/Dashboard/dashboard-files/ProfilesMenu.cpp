#include "ProfilesMenu.h"

ProfilesMenu::ProfilesMenu() {}

void ProfilesMenu::create_menu() {
    // Call the base class create_menu for common functionality
    lv_obj_t* profiles_screen = lv_obj_create(NULL); // Create a new screen

    Menu::init_menu(profiles_screen);

    // Create title label
    lv_obj_t* title_label = lv_label_create(profiles_screen);
    lv_label_set_text(title_label, "Profiles Menu");
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);

    //back button
    lv_obj_t* back_btn = lv_btn_create(profiles_screen);
    lv_obj_set_size(back_btn, 100, 50);
    lv_obj_align(back_btn, LV_ALIGN_TOP_RIGHT, -30, +30);

    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_event_handler, LV_EVENT_CLICKED, NULL);


    // Define the user names for the button matrix
    static const char* btnm_map[] = {
        "Aman", "Blake", "Tyler", "\n",
        "Person 1", "Person 2", "Person 3", ""
    };

    // Create the button matrix
    lv_obj_t* btnm = lv_buttonmatrix_create(profiles_screen);
    lv_buttonmatrix_set_map(btnm, btnm_map);

    // Set the button matrix size (wider and taller)
    lv_obj_set_size(btnm, 400, 300); // Set the new size: width 400px, height 300px
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);  // Center the button matrix on the screen

    // Set all buttons as checkable so only one can be selected at a time
    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 4, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 5, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 6, LV_BUTTONMATRIX_CTRL_CHECKABLE);



    lv_scr_load(profiles_screen);

}

void ProfilesMenu::back_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::menuIndex = -1;
    }

}

