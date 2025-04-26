#include "inc/ConfirmMenu.hpp"

#include "inc/Menu.hpp"

ConfirmMenu::ConfirmMenu(Menu* menu) : Screen(menu) {}

void ConfirmMenu::PostCreate() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame);
    lv_label_set_text(title_label, "Confirm Selection");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    // display selected driver and event
    lv_obj_t* selection_label = lv_label_create(frame);
    lv_label_set_text_fmt(selection_label, "Driver: %s\nEvent: %s",
                          GetDriverName(menu_->selected_driver),
                          GetEventName(menu_->selected_event));
    lv_obj_align(selection_label, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(selection_label, &lv_font_montserrat_24, 0);

    roller = lv_roller_create(frame);
    lv_roller_set_options(roller, "CONFIRM\nCHANGE", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_width(roller, 300);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_24, 0);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_center(roller);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame);
}

void ConfirmMenu::Update(Button select, Button scroll) {
    bool sel = lv_roller_get_selected(roller);
    if (select.PosEdge()) {
        if (sel == Selection::CONFIRM) {
            menu_->ChangeState(State::WAIT_SELECTION_ACK);
        } else {
            menu_->ChangeState(State::SELECT_DRIVER);
        }
    } else if (scroll.PosEdge()) {
        lv_roller_set_selected(roller, !sel, LV_ANIM_ON);
    }
}
