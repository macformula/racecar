#pragma once

#include <optional>

#include "AcknowledgeConfig.hpp"
#include "Button.hpp"
#include "ConfirmMenu.hpp"
#include "DriveModeMenu.hpp"
#include "ErrorScreen.hpp"
#include "LogoScreen.hpp"
#include "ProfileSelect.hpp"
#include "Shutdown.hpp"
#include "StartDriving.hpp"
#include "StartHV.hpp"
#include "StartMotors.hpp"
#include "StartingHV.hpp"
#include "StartingMotors.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"

class Display {
public:
    using State = generated::can::TxDashStatus::State_t;
    using Profile = generated::can::TxDashStatus::Profile_t;

    Display(Button& enter, Button& scroll, generated::can::VehBus& veh);

    // CAN and Buttons are public so the ScreenUpdate can access them
    Button enter;
    Button scroll;
    generated::can::VehBus& veh_bus;

    Profile selected_profile = Profile::Default;

    void Start();
    void Update(int time_ms);

    void ChangeState(State new_state);
    State GetState() const;

private:
    void InnerChangeState(State new_state);

    std::optional<State> transition_;
    State state_ = State::LOGO;
    Screen* screen_;

    LogoScreen logo_screen;
    ProfileSelect profile_select;
    ConfirmMenu confirm_menu;
    AcknowledgeConfig acknowledge_config;
    StartHV start_hv;
    StartingHV starting_hv;
    StartMotors start_motors;
    StartingMotors starting_motors;
    StartDriving start_driving;
    DriveModeMenu drive_mode;
    Shutdown shutdown;
    ErrorScreen error_screen;
};