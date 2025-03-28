#include "inc/ModeSelect.hpp"

// define the names of the modes here
const char* ModeSelect::modes[ModeSelect::num_modes] = {"Launch", "Skidpad",
                                                        "Endurance"};

lv_obj_t* ModeSelect::modes_roller = nullptr;  // init roller to null

ModeSelect::ModeSelect() {}

void ModeSelect::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* modes_select = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(modes_select);

    // title label
    lv_obj_t* title_label = lv_label_create(modes_select);
    lv_label_set_text(title_label, "Select the Mode");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // modes roller
    modes_roller = lv_roller_create(modes_select);

    // create the modes list in format of "Mode1\nMode2\nMode3"
    // supports maximum 256 characters
    char modes_list[256] = "";

    for (int i = 0; i < num_modes; i++) {
        strcat(modes_list, modes[i]);
        if (i < num_modes - 1) {
            strcat(modes_list, "\n");
        }
    }

    lv_roller_set_options(modes_roller, modes_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(modes_roller, 300);
    lv_obj_set_style_text_font(modes_roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(modes_roller, 5);
    lv_obj_center(modes_roller);

    // up and down buttons
    lv_obj_t* up_btn = lv_btn_create(modes_select);
    lv_obj_set_size(up_btn, 100, 50);
    lv_obj_center(up_btn);
    lv_obj_set_x(up_btn, lv_obj_get_x(up_btn) + 250);
    lv_obj_t* up_btn_label = lv_label_create(up_btn);
    lv_label_set_text(up_btn_label, "Up");
    lv_obj_center(up_btn_label);

    lv_obj_t* down_btn = lv_btn_create(modes_select);
    lv_obj_set_size(down_btn, 100, 50);
    lv_obj_center(down_btn);
    lv_obj_set_x(down_btn, lv_obj_get_x(down_btn) - 250);
    lv_obj_t* down_btn_label = lv_label_create(down_btn);
    lv_label_set_text(down_btn_label, "Down");
    lv_obj_center(down_btn_label);

    // add event handlers
    lv_obj_add_event_cb(up_btn, up_btn_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(down_btn, down_btn_event_handler, LV_EVENT_CLICKED,
                        NULL);

    // back button
    lv_obj_t* confirm_btn = lv_btn_create(modes_select);
    lv_obj_set_size(confirm_btn, 100, 50);
    lv_obj_center(confirm_btn);
    lv_obj_set_y(confirm_btn, lv_obj_get_y(confirm_btn) + 175);
    lv_obj_t* confirm_btn_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_btn_label, "Confirm");
    lv_obj_center(confirm_btn_label);
    lv_obj_add_event_cb(confirm_btn, confirm_btn_event_handler,
                        LV_EVENT_CLICKED, NULL);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(modes_select);
}

// proceeds to next screen
void ModeSelect::confirm_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = STATE_CONFIRM;

        // Same function as DriverSelect.cpp, read comments there
        selected_mode = 1;
    }
}

// moves the roller up
void ModeSelect::up_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(modes_roller);
    if (selected > 0) {
        lv_roller_set_selected(modes_roller, selected - 1, LV_ANIM_ON);
    }
}

// moves the roller down
void ModeSelect::down_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(modes_roller);
    if (selected < num_modes - 1) {
        lv_roller_set_selected(modes_roller, selected + 1, LV_ANIM_ON);
    }
}
