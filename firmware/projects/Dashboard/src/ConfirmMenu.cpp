#include "inc/ConfirmMenu.hpp"

#include "inc/Display.hpp"

ConfirmMenu::ConfirmMenu(Display* display) : Screen(display) {}

void ConfirmMenu::CreateGUI() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Confirm Selection");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // display selected driver and event
    lv_obj_t* selection_label = lv_label_create(frame_);
    lv_label_set_text_fmt(selection_label, "Driver: %s\nEvent: %s",
                          GetDriverName(display_->selected_driver),
                          GetEventName(display_->selected_event));
    lv_obj_align(selection_label, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(selection_label, &lv_font_montserrat_24, 0);

    roller = lv_roller_create(frame_);
    lv_roller_set_options(roller, "CONFIRM\nCHANGE", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_center(roller);
}

void ConfirmMenu::Update() {
    bool sel = lv_roller_get_selected(roller);

    if (display_->enter.PosEdge()) {
        if (sel == Selection::CONFIRM) {
            display_->ChangeState(State::WAIT_SELECTION_ACK);
        } else {
            display_->ChangeState(State::SELECT_DRIVER);
        }
    } else if (display_->scroll.PosEdge()) {
        lv_roller_set_selected(roller, !sel, LV_ANIM_ON);
    }
}
