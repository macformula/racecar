#include "inc/DriverSelect.hpp"

// define the names of the drivers here
const char* DriverSelect::drivers[DriverSelect::num_drivers] = {
    "Ada", "Lin", "Logan", "Luca", "Mubashir", "Nathan", "Tyler"};

lv_obj_t* DriverSelect::driver_roller = nullptr;  // init roller to null

DriverSelect::DriverSelect() {}

void DriverSelect::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* driver_select = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(driver_select);

    // title label
    lv_obj_t* title_label = lv_label_create(driver_select);
    lv_label_set_text(title_label, "Select the Driver");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // driver roller
    driver_roller = lv_roller_create(driver_select);

    // create the drivers list in format of "Driver1\nDriver2\nDriver3"
    // supports maximum 256 characters
    char driver_list[256] = "";

    for (int i = 0; i < num_drivers; i++) {
        strcat(driver_list, drivers[i]);
        if (i < num_drivers - 1) {
            strcat(driver_list, "\n");
        }
    }

    lv_roller_set_options(driver_roller, driver_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(driver_roller, 300);
    lv_obj_set_style_text_font(driver_roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(driver_roller, 5);
    lv_obj_center(driver_roller);

    // up and down buttons
    lv_obj_t* up_btn = lv_btn_create(driver_select);
    lv_obj_set_size(up_btn, 100, 50);
    lv_obj_center(up_btn);
    lv_obj_set_x(up_btn, lv_obj_get_x(up_btn) + 250);
    lv_obj_t* up_btn_label = lv_label_create(up_btn);
    lv_label_set_text(up_btn_label, "Up");
    lv_obj_center(up_btn_label);

    lv_obj_t* down_btn = lv_btn_create(driver_select);
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
    lv_obj_t* confirm_btn = lv_btn_create(driver_select);
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
    lv_scr_load(driver_select);
}

// proceeds to next screen
void DriverSelect::confirm_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = State::SELECT_EVENT;

        // Map the roller input to the driver data you want to send via CAN
        // e.g say all the driver data is stored in an array of objects called
        // drivers_data and its stored in the same order as drivers then just
        // have if statements checking if it matches, if so set selected_drivers
        // to the correct drivers_data then in the main function access
        // drivers_data and send it via CAN Not implemented yet because I'm
        // unsure what data types and format we are sending and recieving

        /*if(lv_roller_get_selected(roller) == drivers[0]){
            selected_driver = drivers_data[0]
        }
            ...
        */

        // for now, just setting selected_driver = 1;
        selected_driver = 1;
    }
}

// moves the roller up
void DriverSelect::up_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(driver_roller);
    if (selected > 0) {
        lv_roller_set_selected(driver_roller, selected - 1, LV_ANIM_ON);
    }
}

// moves the roller down
void DriverSelect::down_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(driver_roller);
    if (selected < num_drivers - 1) {
        lv_roller_set_selected(driver_roller, selected + 1, LV_ANIM_ON);
    }
}
