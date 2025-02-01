#include "inc/ProfilesMenu.hpp"

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

    // back button
    lv_obj_t* back_btn = lv_btn_create(profiles_screen);
    lv_obj_set_size(back_btn, 100, 50);
    lv_obj_align(back_btn, LV_ALIGN_TOP_RIGHT, -30, +30);

    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_event_handler, LV_EVENT_CLICKED, NULL);

    // Define the user names for individual buttons
    const char* button_labels[] = { "Aman", "Blake", "Tyler", "Person 1", "Person 2", "Person 3" };

    // Create individual buttons
    lv_obj_t* buttons[6];
    for (int i = 0; i < 6; i++) {
        buttons[i] = lv_btn_create(profiles_screen);
        lv_obj_t* label = lv_label_create(buttons[i]);
        lv_label_set_text(label, button_labels[i]);
        lv_obj_center(label);

        // Set size and position of each button
        lv_obj_set_size(buttons[i], 200, 50); // Button size
        lv_obj_align(buttons[i], LV_ALIGN_CENTER, 0, (i - 2) * 60); // Arrange vertically with spacing
    }

    lv_obj_clean(lv_scr_act());
    lv_scr_load(profiles_screen);
}

void ProfilesMenu::back_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = STATE_DASHBOARD;
    }
}
