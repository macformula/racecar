#include "inc/Display.hpp"

Display::Display(Button& enter, Button& scroll, generated::can::VehBus& veh)
    : enter(enter),
      scroll(scroll),
      veh_bus(veh),
      logo_screen(this),
      profile_select(this),
      confirm_menu(this),
      acknowledge_config(this),
      start_hv(this),
      starting_hv(this),
      start_motors(this),
      starting_motors(this),
      start_driving(this),
      drive_mode(this),
      screen_(&logo_screen) {}

Display::State Display::GetState() const {
    return state_;
}

void Display::ChangeState(State new_state_) {
    // Don't immediately construct the new screen, only set a flag.
    // This prevents a screen from deleting itself during its Update().
    transition_ = new_state_;
}

void Display::Start() {
    // Create the initial screen (as chosen in the constructor)
    screen_->Create();
}

void Display::Update(int time_ms) {
    enter.Update(time_ms);
    scroll.Update(time_ms);

    screen_->Update();

    if (transition_.has_value()) {
        InnerChangeState(transition_.value());
        transition_ = std::nullopt;
    }

    lv_timer_handler();
}

void Display::InnerChangeState(State new_state_) {
    lv_obj_t* old_screen = lv_scr_act();
    state_ = new_state_;

    switch (state_) {
        using enum State;
            // clang-format off
        case LOGO:                  screen_ = &logo_screen; break;
        case SELECT_PROFILE:        screen_ = &profile_select; break;
        case CONFIRM_SELECTION:     screen_ = &confirm_menu; break;
        case WAIT_SELECTION_ACK:    screen_ = &acknowledge_config; break;
        case PRESS_FOR_HV:          screen_ = &start_hv; break;
        case STARTING_HV:           screen_ = &starting_hv; break;
        case PRESS_FOR_MOTOR:       screen_ = &start_motors; break;
        case STARTING_MOTORS:       screen_ = &starting_motors; break;
        case BRAKE_TO_START:        screen_ = &start_driving; break;
        case RUNNING:               screen_ = &drive_mode; break;
            // clang-format on
    }

    screen_->Create();
    lv_obj_del(old_screen);
}