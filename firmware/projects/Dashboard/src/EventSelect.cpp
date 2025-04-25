#include "inc/EventSelect.hpp"

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

    // up and down buttons
    lv_obj_t* up_btn = lv_btn_create(frame);
    lv_obj_set_size(up_btn, 100, 50);
    lv_obj_center(up_btn);
    lv_obj_set_x(up_btn, lv_obj_get_x(up_btn) + 250);
    lv_obj_t* up_btn_label = lv_label_create(up_btn);
    lv_label_set_text(up_btn_label, "Up");
    lv_obj_center(up_btn_label);

    lv_obj_t* down_btn = lv_btn_create(frame);
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
    lv_obj_t* confirm_btn = lv_btn_create(frame);
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
    lv_scr_load(frame);
}

// proceeds to next screen
void EventSelect::confirm_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = State::CONFIRM_SELECTION;

        Menu::selected_event =
            static_cast<Menu::Event>(lv_roller_get_selected(roller));
    }
}

// moves the roller up
void EventSelect::up_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(roller);
    if (selected > 0) {
        lv_roller_set_selected(roller, selected - 1, LV_ANIM_ON);
    }
}

// moves the roller down
void EventSelect::down_btn_event_handler(lv_event_t* e) {
    int selected = lv_roller_get_selected(roller);
    if (selected < kEventCount - 1) {
        lv_roller_set_selected(roller, selected + 1, LV_ANIM_ON);
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