#include "Shutdown.hpp"

#include "Display.hpp"
#include "lvgl.h"

Shutdown::Shutdown(Display* display) : Screen(display) {}

void Shutdown::CreateGUI(void) {
    // title
    lv_obj_t* title_label = lv_label_create(frame_);
    lv_label_set_text(title_label, "Shutting Down...");
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_38, 0);

    if (display_->shutdown_reason ==
        Display::ShutdownReason::CONTACTOR_MISMATCH) {
        lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -30);

        lv_obj_t* reason_label = lv_label_create(frame_);
        lv_label_set_text(reason_label, "Unexpected contactor state");
        lv_obj_align(reason_label, LV_ALIGN_CENTER, 0, 30);
        lv_obj_set_style_text_font(reason_label, &lv_font_montserrat_24, 0);
    }
}

void Shutdown::Update(void) {
    // Reset logic is handled in Display
}