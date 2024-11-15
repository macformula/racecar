#include <Windows.h>
#include <LvglWindowsIconResource.h>
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include <stdio.h>
#include <iostream>

#include "dashboard/drive_menu.h"
#include "dashboard/DashboardMenu.h"
#include "dashboard/DriveModeMenu.h"
#include "dashboard/DebugMenu.h"
#include "dashboard/ProfilesMenu.h"

int main()
{
    lv_init();

    /*
     * If you want to see UTF-8 characters in your console output,
     * uncomment the lines below. Otherwise, feel free to skip them.
     *
     * Credit to jinsc123654 for this tip.
     */
#if LV_TXT_ENC == LV_TXT_ENC_UTF8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Set up display parameters
    int32_t zoom_level = 100;
    bool allow_dpi_override = false;
    bool simulator_mode = true;

    // Create a display for the LVGL Windows simulator
    lv_display_t* display = lv_windows_create_display(
        L"LVGL Windows Simulator Display 1",
        800,
        480,
        zoom_level,
        allow_dpi_override,
        simulator_mode);

    if (!display) {
        return -1; // Exit if display creation failed
    }

    // Get the window handle for further customization
    HWND window_handle = lv_windows_get_display_window_handle(display);
    if (!window_handle) {
        return -1; // Exit if window handle retrieval failed
    }

    // Load and set the application icon
    HICON icon_handle = LoadIconW(
        GetModuleHandleW(NULL),
        MAKEINTRESOURCE(IDI_LVGL_WINDOWS));
    if (icon_handle) {
        SendMessageW(window_handle, WM_SETICON, TRUE, (LPARAM)icon_handle);
        SendMessageW(window_handle, WM_SETICON, FALSE, (LPARAM)icon_handle);
    }

    // Acquire input devices
    lv_indev_t* pointer_indev = lv_windows_acquire_pointer_indev(display);
    if (!pointer_indev) {
        return -1; // Exit if pointer input acquisition failed
    }

    lv_indev_t* keypad_indev = lv_windows_acquire_keypad_indev(display);
    if (!keypad_indev) {
        return -1; // Exit if keypad input acquisition failed
    }

    lv_indev_t* encoder_indev = lv_windows_acquire_encoder_indev(display);
    if (!encoder_indev) {
        return -1; // Exit if encoder input acquisition failed
    }

    // Set up the dashboard
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    DashboardMenu dashboard_menu;
    DriveModeMenu drive_menu;
    DebugMenu debug_menu;
    ProfilesMenu profiles_menu;

    dashboard_menu.create_menu();

    // Main loop for handling events and updates
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next); // Wait until the next event

        if (dashboard_menu.menuIndex == -1) {

            dashboard_menu.menuIndex = 0;
            dashboard_menu.create_menu();
        }

        else if (dashboard_menu.menuIndex == 1) {
            dashboard_menu.menuIndex = 0;
            drive_menu.create_menu();
        }

        else if(dashboard_menu.menuIndex == 2){
            dashboard_menu.menuIndex = 0;
            debug_menu.create_menu();
        }

        else if (dashboard_menu.menuIndex == 3) {
            dashboard_menu.menuIndex = 0;
            profiles_menu.create_menu();
        }

    }

    return 0; // Exit the program
}

