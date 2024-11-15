#include "lvgl.h"
#include "drive_menu.h"

// Global variables to track speed and direction
static int speed = 0;
static bool increasing = true;

// Callback function to update the speedometer
void speed_update_cb(lv_timer_t* timer)
{
    // Retrieve the speed label and speedometer from the timer's user data
    lv_obj_t** objects = (lv_obj_t**)lv_timer_get_user_data(timer);
    lv_obj_t* speed_label = objects[0];
    lv_obj_t* speedometer = objects[1];

    const int MAX_SPEED = 240; // Maximum speed limit
    const int MIN_SPEED = 0;   // Minimum speed limit

    // Adjust speed based on whether we're accelerating or decelerating
    if (increasing) {
        speed += (MAX_SPEED - (MAX_SPEED - speed)) / 20 + 1; // Increase speed
        if (speed >= MAX_SPEED) {
            increasing = false; // Switch to deceleration if max speed is reached
            speed = MAX_SPEED;
        }
    }
    else {
        speed -= speed / 20 + 1; // Decrease speed
        if (speed <= MIN_SPEED) {
            increasing = true; // Switch to acceleration if min speed is reached
            speed = MIN_SPEED;
        }
    }

    // Update the speedometer and the label
    lv_arc_set_value(speedometer, speed);
    char speed_buf[10];
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d", speed);
    lv_label_set_text(speed_label, speed_buf);
}


// Function to create the drive screen
void create_drive_screen() {
    lv_obj_t* drive_screen = lv_obj_create(NULL); // Create a new screen

    // Add a label to indicate drive 
    lv_obj_t* label = lv_label_create(drive_screen);
    lv_label_set_text(label, "ECO Mode Menu");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // You can add more widgets specific to the ECO screen here

    // Load the new screen
    lv_scr_load(drive_screen);
}

// Event handler for drive mode button
void drive_mode_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Call the function to create and load the drive screen
        create_drive_screen();
    }
}

// Function to create the dashboard UI
void create_dashboard(lv_obj_t* parent) {
    char speed_buf[10];

    // Create a background rectangle for the dashboard
    lv_obj_t* rect = lv_obj_create(parent);
    lv_obj_set_size(rect, 800 - 20, 480 - 20); // Reduce size for margins
    lv_obj_align(rect, LV_ALIGN_CENTER, 0, 0); // Center the rectangle
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xD3D3D3), LV_PART_MAIN); // Light grey color
    lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, LV_PART_MAIN); // Fully opaque

    // Title label at the top
    lv_obj_t* title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "Car Dashboard");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);

    // Speedometer arc on the left side
    lv_obj_t* speedometer = lv_arc_create(parent);
    lv_obj_set_size(speedometer, 180, 180);
    lv_arc_set_range(speedometer, 0, 240);
    lv_arc_set_value(speedometer, speed);
    lv_obj_align(speedometer, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_remove_style(speedometer, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(speedometer, LV_OBJ_FLAG_CLICKABLE);

    // Speed label showing the current speed
    lv_obj_t* speed_label = lv_label_create(parent);
    lv_snprintf(speed_buf, sizeof(speed_buf), "%d km/h", speed);
    lv_label_set_text(speed_label, speed_buf);
    lv_obj_align(speed_label, LV_ALIGN_LEFT_MID, 100, 70);

    // Store the label and speedometer for later updates
    static lv_obj_t* timer_objects[2];
    timer_objects[0] = speed_label;
    timer_objects[1] = speedometer;

    // Timer to update speed every 50 milliseconds
    lv_timer_create(speed_update_cb, 50, timer_objects);

    //drive mode button
    lv_obj_t* drive_mode_btn = lv_btn_create(parent);
    lv_obj_set_size(drive_mode_btn, 80, 40);
    lv_obj_align(drive_mode_btn, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_t* eco_label = lv_label_create(drive_mode_btn);
    lv_label_set_text(eco_label, "Drive Mode");
    lv_obj_center(eco_label);

    // Attach the event handler drive mode button
    lv_obj_add_event_cb(drive_mode_btn, drive_mode_btn_event_handler, LV_EVENT_CLICKED, NULL);
}
