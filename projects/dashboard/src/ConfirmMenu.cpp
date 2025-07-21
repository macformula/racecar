#include "ConfirmMenu.hpp"

#include "Display.hpp"

ConfirmMenu::ConfirmMenu(Display* display) : Screen(display) {}

void ConfirmMenu::CreateGUI() {
    // display selected driver and event
    lv_obj_t* selection_label = lv_label_create(frame_);
    lv_label_set_text_fmt(selection_label, "Profile: %s",
                          GetProfileName(display_->selected_profile));
    lv_obj_align(selection_label, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(selection_label, &lv_font_montserrat_48, 0);

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
            display_->ChangeState(State::SELECT_PROFILE);
        }
    } else if (display_->scroll.PosEdge()) {
        lv_roller_set_selected(roller, !sel, LV_ANIM_ON);
    }
}
