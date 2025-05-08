#include "inc/DriverSelect.hpp"

#include "inc/Display.hpp"

DriverSelect::DriverSelect(Display* display) : Screen(display) {}

void DriverSelect::CreateGUI() {
    display_->selected_driver = Display::Driver::UNSPECIFIED;

    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Select the Driver");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // driver roller
    roller = lv_roller_create(frame_);

    // create the drivers list in format of "Driver1\nDriver2\nDriver3"
    char driver_list[256] = "";

    for (int i = 0; i < kNumDrivers; i++) {
        if (i > 0) {
            strcat(driver_list, "\n");
        }
        strcat(driver_list, GetDriverName(static_cast<Display::Driver>(i)));
    }

    lv_roller_set_options(roller, driver_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 5);
    lv_obj_center(roller);
}

void DriverSelect::Update() {
    if (display_->enter.PosEdge()) {
        display_->selected_driver =
            static_cast<Display::Driver>(lv_roller_get_selected(roller));

        display_->ChangeState(State::SELECT_EVENT);

    } else if (display_->scroll.PosEdge()) {
        int new_position = lv_roller_get_selected(roller) + 1;
        new_position = new_position % kNumDrivers;
        lv_roller_set_selected(roller, new_position, LV_ANIM_ON);
    }
}

const char* GetDriverName(Display::Driver d) {
    using enum Display::Driver;
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