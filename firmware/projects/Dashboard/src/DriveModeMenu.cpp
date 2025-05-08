#include "inc/DriveModeMenu.hpp"

#include "inc/Display.hpp"

DriveModeMenu::DriveModeMenu(Display* display) : Screen(display) {}

void DriveModeMenu::CreateGUI() {
    // Speedometer arc
    speedometer_arc = lv_arc_create(frame_);
    lv_obj_set_size(speedometer_arc, 300, 300);
    lv_arc_set_range(speedometer_arc, 0, kArcMaxSpeed * kArcSpeedResolution);
    lv_obj_align(speedometer_arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_remove_style(speedometer_arc, NULL, LV_PART_KNOB);

    // Speed label
    speed_label = lv_label_create(frame_);
    lv_label_set_text(speed_label, "0");
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
}

void DriveModeMenu::Update() {
    auto fc_msg = display_->veh_bus.GetRxFCDashboardStatus();

    if (fc_msg.has_value()) {
        float speed = fc_msg->speed();
        lv_arc_set_value(speedometer_arc, speed * kArcSpeedResolution);
        lv_label_set_text_fmt(speed_label, "%d", static_cast<int>(speed));
    }
}