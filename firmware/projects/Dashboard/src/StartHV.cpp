#include "inc/StartHV.hpp"

#include "generated/can/veh_bus.hpp"
#include "lvgl/lvgl.h"

StartHV::StartHV() {}

void StartHV::create_menu() {
    // calls base class functionality, handles background and frame
    lv_obj_t* start_hv_menu = lv_obj_create(NULL);  // Create a new screen
    Menu::init_menu(start_hv_menu);

    // title label
    lv_obj_t* title_label = lv_label_create(start_hv_menu);
    lv_label_set_text(title_label, "Press SELECT to start HV");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    lv_obj_t* led1 = lv_obj_create(start_hv_menu);
    lv_obj_set_size(led1, 30, 30);
    lv_obj_set_style_radius(led1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(led1, LV_ALIGN_BOTTOM_MID, -60, -30);

    lv_obj_t* led2 = lv_obj_create(start_hv_menu);
    lv_obj_set_size(led2, 30, 30);
    lv_obj_set_style_radius(led2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led2, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(led2, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_obj_t* led3 = lv_obj_create(start_hv_menu);
    lv_obj_set_size(led3, 30, 30);
    lv_obj_set_style_radius(led3, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led3, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(led3, LV_ALIGN_BOTTOM_MID, 60, -30);

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(start_hv_menu);
}

void StartHV::Update(Button select, Button scroll) {
    if (select.PosEdge()) {
        dashboard_state = State::STARTING_HV;
    }
}
