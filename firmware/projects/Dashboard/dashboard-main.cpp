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
#include "etl/include/etl/fsm.h"
#include "etl/include/etl/message_router.h"
#include "etl/include/etl/message.h"

const etl::message_router_id_t FSM_CONTROL = 0;

//FSM Event IDs
struct EventId {
    enum {
        TO_DASHBOARD, //going back to the main menu
        TO_DEBUG, //going to debug menu
        TO_DRIVE, //going to drive menu
        TO_PROFILE //going to profile menu
    };
};

//Event messages
class ToDashboard : public etl::message<EventId::TO_DASHBOARD> {};
class ToDebug : public etl::message<EventId::TO_DEBUG> {};
class ToDrive : public etl::message<EventId::TO_DRIVE> {};
class ToProfile : public etl::message<EventId::TO_PROFILE> {};


//State IDs
struct StateId {
    enum {
        DASHBOARD_MENU_STATE,
        DEBUG_MENU_STATE,
        DRIVE_MENU_STATE,
        PROFILE_MENU_STATE,
        NUMBER_OF_STATES
    };

};

//The dashboard FSM class
class DashboardFSM : public etl::fsm {
public:
    DashboardFSM() : fsm(FSM_CONTROL) {  }
    void LogUnknownEvent(const etl::imessage& msg) {  }
};

// The main menu state. Accepts the ToDebug, ToDrive, and ToProfile events
class DashboardMenuState : public etl::fsm_state<DashboardFSM, DashboardMenuState, StateId::DASHBOARD_MENU_STATE, ToDebug, ToDrive, ToProfile> {
public:
    // Handles the ToDebug event
    etl::fsm_state_id_t on_event(const ToDebug& event) {
        return StateId::DEBUG_MENU_STATE;
    }

    // Handles the ToDrive event
    etl::fsm_state_id_t on_event(const ToDrive& event) {
        return StateId::DRIVE_MENU_STATE;
    }

    // Handles the ToProfile event
    etl::fsm_state_id_t on_event(const ToProfile& event) {
        return StateId::PROFILE_MENU_STATE;
    }

    // Handles unknown events
    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};

//The debug menu state. Accepts the ToDashboard event
class DebugMenuState : public etl::fsm_state<DashboardFSM, DebugMenuState, StateId::DEBUG_MENU_STATE, ToDashboard> {
public:
    //***********************************  
    etl::fsm_state_id_t on_event(const ToDashboard& event) {
        return StateId::DASHBOARD_MENU_STATE;
    }

    //***********************************  
    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};

//The drive menu state. Accepts the ToDashboard event
class DriveMenuState : public etl::fsm_state<DashboardFSM, DriveMenuState, StateId::DRIVE_MENU_STATE, ToDashboard> {
public:
    //***********************************  
    etl::fsm_state_id_t on_event(const ToDashboard& event) {
        return StateId::DASHBOARD_MENU_STATE;
    }

    //***********************************  
    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};

//The profile menu state. Accepts the ToDashboard event
class ProfileMenuState : public etl::fsm_state<DashboardFSM, ProfileMenuState, StateId::PROFILE_MENU_STATE, ToDashboard> {
public:
    //***********************************  
    etl::fsm_state_id_t on_event(const ToDashboard& event) {
        return StateId::DASHBOARD_MENU_STATE;
    }

    //***********************************  
    etl::fsm_state_id_t on_event_unknown(const etl::imessage& event) {
        get_fsm_context().LogUnknownEvent(event);
        return STATE_ID;
    }
};







//Main function
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


    //*****************************************************
    //Defining the states
    DashboardMenuState dashboard_menu_state;
    DebugMenuState debug_menu_state;
    DriveMenuState drive_menu_state;
    ProfileMenuState profile_menu_state;

    //Matching states with ID
    etl::ifsm_state* stateList[StateId::NUMBER_OF_STATES] =
    {
        &dashboard_menu_state, &debug_menu_state, &drive_menu_state, & profile_menu_state
    };

    DashboardFSM dashboard_fsm;

    dashboard_fsm.set_states(stateList, 4);

    dashboard_fsm.start();
    dashboard_fsm.receive(ToDashboard());
    

    // Set up the dashboard
    lv_obj_t* dashboard_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dashboard_frame, LV_HOR_RES, LV_VER_RES);

    //Define all the state objects
    DashboardMenu dashboard_menu;
    DriveModeMenu drive_menu;
    DebugMenu debug_menu;
    ProfilesMenu profiles_menu;

    //Set default menu to dashboard menu
    /*
    menuIndex 0: No change
    menuIndex -1: Dashboard Menu
    menuIndex 1: Drive Menu
    menuIndex 2: Debug Menu
    menuIndex 3: Profiles Menu
    */
    dashboard_menu.menuIndex = 0;
    dashboard_menu.create_menu();
    etl::fsm_state_id_t currentState = StateId::DASHBOARD_MENU_STATE;


    //Example: Manually go to a state (e.g profiles)
    //dashboard_fsm.receive(ToProfile());

    // Main loop for handling events and updates
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next); // Wait until the next event

        // Setting states correctly based on input from program
        if (dashboard_menu.menuIndex == -1) {
            dashboard_fsm.receive(ToDashboard());
        }
        else if (dashboard_menu.menuIndex == 1) {
            dashboard_fsm.receive(ToDrive());
        }
        else if (dashboard_menu.menuIndex == 2) {
            dashboard_fsm.receive(ToDebug());
        }
        else if (dashboard_menu.menuIndex == 3) {
            dashboard_fsm.receive(ToProfile());
        }

        // Change menu depending on state
        etl::fsm_state_id_t newState = dashboard_fsm.get_state_id();
        if (newState != currentState) { // Only redraw if the state has changed
            currentState = newState; // Update the current state

            // Create the appropriate menu for the current state
            switch (currentState) {
            case StateId::DASHBOARD_MENU_STATE:
                dashboard_menu.menuIndex = 0;
                dashboard_menu.create_menu();
                break;
            case StateId::DRIVE_MENU_STATE:
                dashboard_menu.menuIndex = 0;
                drive_menu.create_menu();
                break;
            case StateId::DEBUG_MENU_STATE:
                dashboard_menu.menuIndex = 0;
                debug_menu.create_menu();
                break;
            case StateId::PROFILE_MENU_STATE:
                dashboard_menu.menuIndex = 0;
                profiles_menu.create_menu();
                break;
            default:
                break;
            }
        }
    }


    return 0; // Exit the program
}

