#include "inc/EventSelect.hpp"

#include "generated/can/veh_bus.hpp"

lv_obj_t* EventSelect::roller = nullptr;  // init roller to null

EventSelect::EventSelect() {}

void EventSelect::create_menu() {
    Menu::selected_event = Menu::Event::UNSPECIFIED;
    // calls base class functionality, handles background and frame
    lv_obj_t* frame = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(frame);

    // title label
    lv_obj_t* title_label = lv_label_create(frame);
    lv_label_set_text(title_label, "Select the Event");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    roller = lv_roller_create(frame);

    // create a string list "Event1\nEvent2\nEvent3"
    char event_list[256] = "";

    for (int i = 0; i < kEventCount; i++) {
        if (i > 0) {
            strcat(event_list, "\n");
        }
        strcat(event_list, GetEventName(static_cast<Menu::Event>(i)));
    }

    lv_roller_set_options(roller, event_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 5);
    lv_obj_center(roller);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame);
}

void EventSelect::Update(Button select, Button scroll) {
    if (select.PosEdge()) {
        selected_event =
            static_cast<Menu::Event>(lv_roller_get_selected(roller));
        Menu::dashboard_state = State::CONFIRM_SELECTION;
    } else if (scroll.PosEdge()) {
        int new_position = lv_roller_get_selected(roller) + 1;
        new_position %= kEventCount;
        lv_roller_set_selected(roller, new_position, LV_ANIM_ON);
    }
}

// define the names of the modes here
const char* GetEventName(Menu::Event e) {
    using enum Menu::Event;
    // clang-format off
    switch (e) {
        case Launch:        return "Launch";
        case Skidpad:       return "Skidpad";
        case Endurance:     return "Endurance";
        case UNSPECIFIED:
        default:            return "--";
    }
    // clang-format on
}