#include "DebugMenu.h"

DebugMenu::DebugMenu() {}

void DebugMenu::create_menu() {
    // Call the base class create_menu for common functionality
    lv_obj_t* debug_screen = lv_obj_create(NULL); // Create a new screen

    Menu::init_menu(debug_screen);

    // Create title label
    lv_obj_t* title_label = lv_label_create(debug_screen);
    lv_label_set_text(title_label, "Debug Mode");
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);

    //back button
    lv_obj_t* back_btn = lv_btn_create(debug_screen);
    lv_obj_set_size(back_btn, 100, 50);
    lv_obj_align(back_btn, LV_ALIGN_TOP_RIGHT, -30, +30);

    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_btn_event_handler, LV_EVENT_CLICKED, NULL);





    // Engine Temperature Bar (Simulated)
    lv_obj_t* temp_bar = lv_bar_create(debug_screen);
    lv_obj_set_size(temp_bar, 200, 20);
    lv_bar_set_range(temp_bar, 0, 120); // Engine temp range (0-120°C)
    lv_bar_set_value(temp_bar, 85, LV_ANIM_OFF); // Simulated engine temp
    lv_obj_align(temp_bar, LV_ALIGN_TOP_LEFT, 20, 60);

    // Engine Temperature Label
    lv_obj_t* temp_label = lv_label_create(debug_screen);
    lv_label_set_text(temp_label, "Engine Temp");
    lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 20, 90);

    // Battery Voltage Bar (Simulated)
    lv_obj_t* battery_bar = lv_bar_create(debug_screen);
    lv_obj_set_size(battery_bar, 200, 20);
    lv_bar_set_range(battery_bar, 0, 16); // Battery voltage range (0-16V)
    lv_bar_set_value(battery_bar, 12.4 * 10, LV_ANIM_OFF); // Simulated battery voltage, scaled by 10 for bar value
    lv_obj_align(battery_bar, LV_ALIGN_TOP_LEFT, 20, 120);

    // Battery Voltage Label
    lv_obj_t* battery_label = lv_label_create(debug_screen);
    lv_label_set_text(battery_label, "Battery Voltage");
    lv_obj_align(battery_label, LV_ALIGN_TOP_LEFT, 20, 150);

    // Tire Pressure Bar (Simulated for Tire 1)
    lv_obj_t* tire_bar = lv_bar_create(debug_screen);
    lv_obj_set_size(tire_bar, 200, 20);
    lv_bar_set_range(tire_bar, 0, 50); // Tire pressure range (0-50 PSI)
    lv_bar_set_value(tire_bar, 32, LV_ANIM_OFF); // Simulated tire pressure for Tire 1
    lv_obj_align(tire_bar, LV_ALIGN_TOP_LEFT, 20, 180);

    // Tire Pressure Label
    lv_obj_t* tire_label = lv_label_create(debug_screen);
    lv_label_set_text(tire_label, "Tire 1 Pressure");
    lv_obj_align(tire_label, LV_ALIGN_TOP_LEFT, 20, 210);

    // Simulate Tire Service Warning Button (No callback, just visual)
    lv_obj_t* tire_service_btn = lv_btn_create(debug_screen);
    lv_obj_set_size(tire_service_btn, 150, 50);
    lv_obj_align(tire_service_btn, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    lv_obj_t* tire_service_label = lv_label_create(tire_service_btn);
    lv_label_set_text(tire_service_label, "Tire Service");
    lv_obj_center(tire_service_label);

    // Steering Wheel Position Arc (Simulated)
    lv_obj_t* wheel_position_arc = lv_arc_create(debug_screen);
    lv_obj_set_size(wheel_position_arc, 150, 150); // Set arc size
    lv_arc_set_range(wheel_position_arc, -90, 90); // Steering wheel position range (-90 to +90 degrees)
    lv_arc_set_value(wheel_position_arc, 0); // Simulated neutral position (0 degrees)
    lv_obj_align(wheel_position_arc, LV_ALIGN_TOP_LEFT, 20, 240);

    // Wheel Position Label
    lv_obj_t* wheel_position_label = lv_label_create(debug_screen);
    lv_label_set_text(wheel_position_label, "Wheel Position");
    lv_obj_align(wheel_position_label, LV_ALIGN_TOP_LEFT, 20, 390);


    /* Create Engine Performance chart */
    lv_obj_t* engine_chart = lv_chart_create(debug_screen);
    lv_obj_set_size(engine_chart, 200, 100);  // Smaller chart size
    lv_chart_set_type(engine_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(engine_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);  // Range from 0 to 100%
    lv_chart_set_point_count(engine_chart, 6);  // 6 data points
    lv_obj_align(engine_chart, LV_ALIGN_CENTER, 0, -60);  // Position the chart near the top center

    /* Create series for engine performance */
    lv_chart_series_t* engine_series = lv_chart_add_series(engine_chart, lv_palette_lighten(LV_PALETTE_GREEN, 2), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 6; i++) {
        lv_chart_set_next_value(engine_chart, engine_series, lv_rand(40, 90));  // Fake engine performance data
    }

    /* Create Battery Status chart */
    lv_obj_t* battery_chart = lv_chart_create(debug_screen);
    lv_obj_set_size(battery_chart, 200, 100);  // Smaller chart size
    lv_obj_align(battery_chart, LV_ALIGN_CENTER, 0, +60);  // Space below the first chart
    lv_chart_set_type(battery_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(battery_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 14);  // Battery voltage range (0-14V)
    lv_chart_set_point_count(battery_chart, 6);  // 6 data points

    /* Create series for battery status */
    lv_chart_series_t* battery_series = lv_chart_add_series(battery_chart, lv_palette_darken(LV_PALETTE_BLUE, 2), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 6; i++) {
        lv_chart_set_next_value(battery_chart, battery_series, lv_rand(10, 14));  // Fake battery voltage data
    }

    // Create Engine Temperature scale (top)
    lv_obj_t* scale_temp = lv_scale_create(debug_screen);
    lv_obj_set_size(scale_temp, 150, 150);
    lv_scale_set_mode(scale_temp, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale_temp, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale_temp, lv_palette_lighten(LV_PALETTE_RED, 5), 0);
    lv_obj_set_style_radius(scale_temp, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale_temp, true, 0);
    lv_obj_align(scale_temp, LV_ALIGN_TOP_RIGHT, -20, 95);

    lv_scale_set_label_show(scale_temp, true);
    lv_scale_set_total_tick_count(scale_temp, 31);
    lv_scale_set_major_tick_every(scale_temp, 5);
    lv_obj_set_style_length(scale_temp, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale_temp, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale_temp, 0, 100);  // 0-100 for temperature

    lv_scale_set_angle_range(scale_temp, 270);
    lv_scale_set_rotation(scale_temp, 135);

    // Set the needle for Engine Temperature (e.g., 70°C)
    lv_obj_t* needle_temp = lv_line_create(scale_temp);
    lv_obj_set_style_line_width(needle_temp, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_temp, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(scale_temp, needle_temp, 60, 70);  // Example fixed value (70°C)

    // Create Battery Voltage scale (bottom)
    lv_obj_t* scale_voltage = lv_scale_create(debug_screen);
    lv_obj_set_size(scale_voltage, 150, 150);
    lv_scale_set_mode(scale_voltage, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale_voltage, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale_voltage, lv_palette_lighten(LV_PALETTE_GREEN, 5), 0);
    lv_obj_set_style_radius(scale_voltage, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale_voltage, true, 0);
    lv_obj_align(scale_voltage, LV_ALIGN_BOTTOM_RIGHT, -20, -70); // Adjusted offset

    lv_scale_set_label_show(scale_voltage, true);
    lv_scale_set_total_tick_count(scale_voltage, 31);
    lv_scale_set_major_tick_every(scale_voltage, 5);
    lv_obj_set_style_length(scale_voltage, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale_voltage, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale_voltage, 10, 16);  // Voltage range (e.g., 10-16V for a car battery)

    lv_scale_set_angle_range(scale_voltage, 270);
    lv_scale_set_rotation(scale_voltage, 135);

    // Set the needle for Battery Voltage (e.g., 12V)
    lv_obj_t* needle_voltage = lv_line_create(scale_voltage);
    lv_obj_set_style_line_width(needle_voltage, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_voltage, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(scale_voltage, needle_voltage, 10, 12);  // Example fixed value (12V)



    lv_scr_load(debug_screen);

}

void DebugMenu::back_btn_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Menu::menuIndex = -1;
    }

}

