#include "inc/EventSelect.hpp"

#include "generated/can/veh_bus.hpp"
#include "inc/Display.hpp"

EventSelect::EventSelect(Display* display) : Screen(display) {}

void EventSelect::CreateGUI() {
    display_->selected_event = Event::UNSPECIFIED;
    // calls base class functionality, handles background and frame

    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Select the Event");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    roller = lv_roller_create(frame_);

    // create a string list "Event1\nEvent2\nEvent3"
    char event_list[256] = "";

    for (int i = 0; i < kEventCount; i++) {
        if (i > 0) {
            strcat(event_list, "\n");
        }
        strcat(event_list, GetEventName(static_cast<Event>(i)));
    }

    lv_roller_set_options(roller, event_list, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 5);
    lv_obj_center(roller);
}

void EventSelect::Update() {
    if (display_->enter.PosEdge()) {
        display_->selected_event =
            static_cast<Event>(lv_roller_get_selected(roller));
        display_->ChangeState(State::CONFIRM_SELECTION);
    } else if (display_->scroll.PosEdge()) {
        int new_position = lv_roller_get_selected(roller) + 1;
        new_position %= kEventCount;
        lv_roller_set_selected(roller, new_position, LV_ANIM_ON);
    }
}

// define the names of the modes here
const char* GetEventName(Display::Event e) {
    using enum Display::Event;
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