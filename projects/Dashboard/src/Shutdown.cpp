#include "inc/Shutdown.hpp"

#include "inc/Display.hpp"
#include "lvgl.h"

Shutdown::Shutdown(Display* display) : Screen(display) {}

void Shutdown::CreateGUI(void) {
    // title
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Shutting Down...");
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);
}

void Shutdown::Update(void) {
    // Reset logic is handled in Display
}