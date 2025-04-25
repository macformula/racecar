#include "inc/DriverSelect.hpp"

#include "inc/Menu.hpp"

lv_obj_t* DriverSelect::roller = nullptr;  // init roller to null

DriverSelect::DriverSelect() {}

void DriverSelect::create_menu() {
    Menu::selected_driver = Menu::Driver::UNSPECIFIED;

    // calls base class functionality, handles background and frame
    lv_obj_t* driver_select = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(driver_select);

    // title label
    lv_obj_t* title_label = lv_label_create(driver_select);
    lv_label_set_text(title_label, "Select the Driver");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // driver roller
    roller = lv_roller_create(driver_select);

    // create the drivers list in format of "Driver1\nDriver2\nDriver3"
    char driver_list[256] = "";

    for (int i = 0; i < kNumDrivers; i++) {
        if (i > 0) {
            strcat(driver_list, "\n");
        }
        strcat(driver_list, GetDriverName(static_cast<Menu::Driver>(i)));
    }

    lv_roller_set_options(roller, driver_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 5);
    lv_obj_center(roller);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(driver_select);
}

void DriverSelect::Update(Button select, Button scroll) {
    if (select.PosEdge()) {
        selected_driver =
            static_cast<Menu::Driver>(lv_roller_get_selected(roller));

        Menu::dashboard_state = State::SELECT_EVENT;
    } else if (scroll.PosEdge()) {
        int new_position = lv_roller_get_selected(roller) + 1;
        new_position = new_position % kNumDrivers;
        lv_roller_set_selected(roller, new_position, LV_ANIM_ON);
    }
}

const char* GetDriverName(Menu::Driver d) {
    using enum Menu::Driver;
    // clang-format off
    switch (d) {
        case Ada:           return "Ada";
        case Lin:           return "Lin";
        case Logan:         return "Logan";
        case Luca:          return "Luca";
        case Mubashir:      return "Mubashir";
        case Nathan:        return "Nathan";
        case Tyler:         return "Tyler";
        case UNSPECIFIED:
        default:            return "ERROR"; // this should not be displayed
    }
    // clang-format on
}