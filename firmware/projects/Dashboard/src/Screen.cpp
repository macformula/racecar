#include "inc/Screen.hpp"

#include "lvgl.h"

Screen::Screen(Display* display) : display_(display) {}

void Screen::Create() {
    frame_ = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(frame_, lv_color_hex(0xd3d3d3), LV_PART_MAIN);

    CreateGUI();  // Screen-specific GUI elements

    // cleanup and load screen
    lv_obj_clean(lv_scr_act());
    lv_scr_load(frame_);
}