#include "inc/DriveModeMenu.hpp"

int DriveModeMenu::speed = 0;
bool DriveModeMenu::increasing = true;
lv_obj_t* DriveModeMenu::drive_screen = NULL;
lv_timer_t* DriveModeMenu::speed_timer = NULL;


DriveModeMenu::DriveModeMenu() {}

void DriveModeMenu::create_menu() {

    speed = 0;
    increasing = 0;


    // Call the base class create_menu for common functionality
    drive_screen = lv_obj_create(NULL); // Create a new screen

    Menu::init_menu(drive_screen);

    

    // Create title label
    lv_obj_t* title_label = lv_label_create(drive_screen);
    lv_label_set_text(title_label, "Drive Mode");
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);

    char speed_buf[10];

    //spedometer box
    // Create the dark background box behind the speedometer
    lv_obj_t* speedometer_box = lv_obj_create(drive_screen);
    lv_obj_set_size(speedometer_box, 200, 200);  // Slightly larger than the speedometer to fully cover it
    lv_obj_set_style_bg_color(speedometer_box, lv_color_hex(0x333333), 0);  // Dark gray background
    lv_obj_set_style_bg_opa(speedometer_box, LV_OPA_COVER, 0);  // Fully opaque
    lv_obj_set_style_border_width(speedometer_box, 0, 0);  // No border
    lv_obj_align(speedometer_box, LV_ALIGN_LEFT_MID, 31, -5);  // Align the box behind the speedometer


    // Create the speedometer and label
    lv_obj_t* speedometer = lv_arc_create(drive_screen);
    lv_obj_set_size(speedometer, 180, 180);
    lv_arc_set_range(speedometer, 0, 240);
    lv_arc_set_value(speedometer, speed);
    lv_obj_align(speedometer, LV_ALIGN_LEFT_MID, 40, 0);
    lv_obj_remove_style(speedometer, NULL, LV_PART_KNOB);

    lv_obj_t* speed_label = lv_label_create(drive_screen);
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d km/h", speed);
    lv_label_set_text(speed_label, speed_buf);
    lv_obj_align(speed_label, LV_ALIGN_LEFT_MID, 120, 70);
    lv_obj_set_style_text_color(speed_label, lv_color_hex(0xFFFFFF), 0);  // White text


    // Store the label and speedometer for later updates
    static lv_obj_t* timer_objects[2];
    timer_objects[0] = speed_label;
    timer_objects[1] = speedometer;

    //back button
    lv_obj_t* back_btn = lv_btn_create(drive_screen);
    lv_obj_set_size(back_btn, 100, 50);
    lv_obj_align(back_btn, LV_ALIGN_TOP_RIGHT, -30, +30);

    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_event_handler, LV_EVENT_CLICKED, NULL);

    //indicator leds
    // Create the first LED indicator and label
    lv_obj_t* led1 = lv_led_create(drive_screen);
    lv_obj_set_size(led1, 20, 20);
    lv_obj_align(led1, LV_ALIGN_BOTTOM_LEFT, 20+20, -40);  // Align the first LED
    lv_led_off(led1); // Start with LED off

    lv_obj_t* label1 = lv_label_create(drive_screen);
    lv_label_set_text(label1, "ENG");
    lv_obj_align(label1, LV_ALIGN_BOTTOM_LEFT, 20+20, -20);  // Label below the first LED

    // Create the second LED indicator and label
    lv_obj_t* led2 = lv_led_create(drive_screen);
    lv_obj_set_size(led2, 20, 20);
    lv_obj_align(led2, LV_ALIGN_BOTTOM_LEFT, 60+20, -40);  // Align the second LED
    lv_led_on(led2); // Start with LED off

    lv_obj_t* label2 = lv_label_create(drive_screen);
    lv_label_set_text(label2, "OIL");
    lv_obj_align(label2, LV_ALIGN_BOTTOM_LEFT, 60+20, -20);  // Label below the second LED

    // Create the third LED indicator and label
    lv_obj_t* led3 = lv_led_create(drive_screen);
    lv_obj_set_size(led3, 20, 20);
    lv_obj_align(led3, LV_ALIGN_BOTTOM_LEFT, 100+20, -40);  // Align the third LED
    lv_led_off(led3); // Start with LED off

    lv_obj_t* label3 = lv_label_create(drive_screen);
    lv_label_set_text(label3, "BAT");
    lv_obj_align(label3, LV_ALIGN_BOTTOM_LEFT, 100+20, -20);  // Label below the third LED

    // Create the fourth LED indicator and label
    lv_obj_t* led4 = lv_led_create(drive_screen);
    lv_obj_set_size(led4, 20, 20);
    lv_obj_align(led4, LV_ALIGN_BOTTOM_LEFT, 140+20, -40);  // Align the fourth LED
    lv_led_on(led4); // Start with LED off
    lv_led_set_color(led4, lv_color_hex(0xff0080));

    lv_obj_t* label4 = lv_label_create(drive_screen);
    lv_label_set_text(label4, "BKE");
    lv_obj_align(label4, LV_ALIGN_BOTTOM_LEFT, 140+20, -20);  // Label below the fourth LED

    // Create the fifth LED indicator and label
    lv_obj_t* led5 = lv_led_create(drive_screen);
    lv_obj_set_size(led5, 20, 20);
    lv_obj_align(led5, LV_ALIGN_BOTTOM_LEFT, 180+20, -40);  // Align the fifth LED
    lv_led_off(led5); // Start with LED off

    lv_obj_t* label5 = lv_label_create(drive_screen);
    lv_label_set_text(label5, "PSI");
    lv_obj_align(label5, LV_ALIGN_BOTTOM_LEFT, 180+20, -20);  // Label below the fifth LED

    // CCAR MODES ROLLER
    const char* car_modes = "Eco\nSport\nComfort\nOff-road\nTrack\nSnow";

    lv_obj_t* car_mode_roller = lv_roller_create(drive_screen);
    lv_roller_set_options(car_mode_roller, car_modes, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(car_mode_roller, 150, 100);  // Adjust size as needed
    lv_obj_align(car_mode_roller, LV_ALIGN_CENTER, 0, 0);  // Center the roller on the screen

    // Set the default selected item to "Comfort" (index 2)
    lv_roller_set_selected(car_mode_roller, 2, LV_ANIM_OFF);

    lv_obj_set_style_bg_color(car_mode_roller, lv_color_hex(0x444444), 0); // Dark background
    lv_obj_set_style_text_color(car_mode_roller, lv_color_hex(0xFFFFFF), 0); // White text

    // Create a label to show the selected car mode (this will display initially as "Eco")
    lv_obj_t* selected_mode_label = lv_label_create(drive_screen);
    lv_label_set_text(selected_mode_label, "Selected Mode: Eco");  // Initial mode
    lv_obj_align(selected_mode_label, LV_ALIGN_BOTTOM_RIGHT, -20, -20);  // Position the label below the roller

    lv_obj_clean(lv_scr_act());
    lv_scr_load(drive_screen);

    // Timer to update speed every 50 milliseconds
    speed_timer = lv_timer_create(speed_update_cb, 50, timer_objects);

}

void DriveModeMenu::speed_update_cb(lv_timer_t* timer) {
    lv_obj_t ** objects = (lv_obj_t **)timer->user_data;

    lv_obj_t* speed_label = objects[0];
    lv_obj_t* speedometer = objects[1];

    const int MAX_SPEED = 240; // Maximum speed limit
    const int MIN_SPEED = 0;   // Minimum speed limit

    // Adjust speed based on whether we're accelerating or decelerating
    if (increasing) {
        speed += (MAX_SPEED - (MAX_SPEED - speed)) / 20 + 1;
        if (speed >= MAX_SPEED) {
            increasing = false;
            speed = MAX_SPEED;
        }
    }
    else {
        speed -= speed / 20 + 1;
        if (speed <= MIN_SPEED) {
            increasing = true;
            speed = MIN_SPEED;
        }
    }

    // Update the speedometer and the label
    lv_arc_set_value(speedometer, speed);
    char speed_buf[10];
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d", speed);
    lv_label_set_text(speed_label, speed_buf);
}

void DriveModeMenu::back_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::dashboard_state = STATE_DASHBOARD;
        lv_timer_del(speed_timer);
    }

}
