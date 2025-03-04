#include "inc/DriveModeMenu.hpp"

int DriveModeMenu::speed = 0;
bool DriveModeMenu::increasing = true;
lv_obj_t* DriveModeMenu::drive_screen = NULL;
lv_timer_t* DriveModeMenu::speed_timer = NULL;

DriveModeMenu::DriveModeMenu() {}

void DriveModeMenu::create_menu() {
    speed = 0;
    increasing = 0;

    drive_screen = lv_obj_create(NULL); // Create a new screen
    Menu::init_menu(drive_screen);

    // Create title label
    lv_obj_t* title_label = lv_label_create(drive_screen);
    lv_label_set_text(title_label, "Drive Mode");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x000000), 0); // Set text to black

    char speed_buf[10];

    // Speedometer background box
    lv_obj_t* speedometer_box = lv_obj_create(drive_screen);
    lv_obj_set_size(speedometer_box, 320, 320);
    lv_obj_set_style_bg_color(speedometer_box, lv_color_hex(0x222222), 0);
    lv_obj_set_style_bg_opa(speedometer_box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(speedometer_box, 0, 0);
    lv_obj_set_style_radius(speedometer_box, 160, 0);
    lv_obj_align(speedometer_box, LV_ALIGN_CENTER, 0, 0);

    // Speedometer arc
    lv_obj_t* speedometer = lv_arc_create(drive_screen);
    lv_obj_set_size(speedometer, 300, 300);
    lv_arc_set_range(speedometer, 0, 240);
    lv_arc_set_value(speedometer, speed);
    lv_obj_align(speedometer, LV_ALIGN_CENTER, 0, 0);
    lv_obj_remove_style(speedometer, NULL, LV_PART_KNOB);

    // Speed label
    lv_obj_t* speed_label = lv_label_create(drive_screen);
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d km/h", speed);
    lv_label_set_text(speed_label, speed_buf);
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_38, 0);

    // Store for updates
    static lv_obj_t* timer_objects[2] = { speed_label, speedometer };

    lv_obj_clean(lv_scr_act());
    lv_scr_load(drive_screen);

    // Timer for speed updates
    speed_timer = lv_timer_create(speed_update_cb, 50, timer_objects);
}

void DriveModeMenu::speed_update_cb(lv_timer_t* timer) {
    lv_obj_t** objects = (lv_obj_t**)timer->user_data;
    lv_obj_t* speed_label = objects[0];
    lv_obj_t* speedometer = objects[1];

    const int MAX_SPEED = 240, MIN_SPEED = 0;

    if (increasing) {
        speed += (MAX_SPEED - speed) / 20 + 1;
        if (speed >= MAX_SPEED) {
            increasing = false;
            speed = MAX_SPEED;
        }
    } else {
        speed -= speed / 20 + 1;
        if (speed <= MIN_SPEED) {
            increasing = true;
            speed = MIN_SPEED;
        }
    }

    lv_arc_set_value(speedometer, speed);
    char speed_buf[10];
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d km/h", speed);
    lv_label_set_text(speed_label, speed_buf);

    // Change speed text color based on speed
    if (speed < 80) {
        lv_obj_set_style_text_color(speed_label, lv_color_hex(0x00FF00), 0); // Green for normal speed
    } else if (speed < 160) {
        lv_obj_set_style_text_color(speed_label, lv_color_hex(0xFFD700), 0); // Yellow for high speed
    } else {
        lv_obj_set_style_text_color(speed_label, lv_color_hex(0xFF0000), 0); // Red for dangerous speed
    }
}
