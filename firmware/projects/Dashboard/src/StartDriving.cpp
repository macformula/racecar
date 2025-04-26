#include "inc/StartDriving.hpp"

#include "inc/Menu.hpp"
#include "lvgl/lvgl.h"

StartDriving::StartDriving(Menu* menu) : Screen(menu) {}

void StartDriving::PostCreate() {
    // title label
    lv_obj_t* title_label = lv_label_create(frame);
    lv_label_set_text(title_label, "Press brakes to start driving!");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    lv_obj_t* led1 = lv_obj_create(frame);
    lv_obj_set_size(led1, 30, 30);
    lv_obj_set_style_radius(led1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(led1, LV_ALIGN_CENTER, -60, 0);  // Offset left by 60px

    lv_obj_t* led2 = lv_obj_create(frame);
    lv_obj_set_size(led2, 30, 30);
    lv_obj_set_style_radius(led2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led2, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(led2, LV_ALIGN_CENTER, 60, 0);  // Offset right by 60px

    lv_obj_t* led3 = lv_obj_create(frame);
    lv_obj_set_size(led3, 30, 30);
    lv_obj_set_style_radius(led3, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(led3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_center(led3);  // Keep center

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame);
}

void StartDriving::Update(Button select, Button scroll) {
    auto fc_msg = menu_->veh_bus.GetRxFCDashboardStatus();
    if (fc_msg.has_value() && fc_msg->driveStarted()) {
        menu_->ChangeState(State::RUNNING);
    }
}
