#include "inc/Screen.hpp"

#include <iostream>

#include "lvgl/lvgl.h"

void Screen::Create() {
    std::cout << "frame" << std::endl;
    frame = lv_obj_create(NULL);
    std::cout << "lv_obj_" << std::endl;
    lv_obj_t* rect = lv_obj_create(frame);
    std::cout << "lv_obj_set_size" << std::endl;
    lv_obj_set_size(rect, 800 - 20, 480 - 20);  // Adjust size for margins
    std::cout << "lv_obj_align" << std::endl;
    lv_obj_align(rect, LV_ALIGN_CENTER, 0, 0);  // Center the rectangle
    std::cout << "lv_obj_set_style_bg_color" << std::endl;
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xD3D3D3), LV_PART_MAIN);
    std::cout << "lv_obj_set_style_bg_opa" << std::endl;
    lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, LV_PART_MAIN);  // Fully opaque

    std::cout << "entering post" << std::endl;

    PostCreate();
}