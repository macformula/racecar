#include "inc/Menu.hpp"

Menu::Menu(generated::can::VehBus& veh)
    : veh_bus(veh),
      logo_screen(this),
      driver_select(this),
      event_select(this),
      confirm_menu(this),
      start_hv(this),
      waiting_screen(this),
      start_motors(this),
      start_driving(this),
      drive_mode(this) {}

void Menu::ChangeState(State new_state_) {
    lv_obj_t* old_screen = lv_scr_act();

    switch (Menu::state_) {
        using enum State;
            // clang-format off
        case LOGO:                  screen_ = &logo_screen; break;
        case SELECT_DRIVER:         screen_ = &driver_select; break;
        case SELECT_EVENT:          screen_ = &event_select; break;
        case CONFIRM_SELECTION:     screen_ = &confirm_menu; break;
        case WAIT_SELECTION_ACK:    screen_ = &waiting_screen; break;
        case PRESS_FOR_HV:          screen_ = &start_hv; break;
        case STARTING_HV:           screen_ = &waiting_screen; break;
        case PRESS_FOR_MOTOR:       screen_ = &start_motors; break;
        case STARTING_MOTOR:        screen_ = &waiting_screen; break;
        case BRAKE_TO_START:        screen_ = &start_driving; break;
        case RUNNING:               screen_ = &drive_mode; break;
        // clang-format on
        default:
            break;
    }

    screen_->Create();
    lv_obj_del(old_screen);
}