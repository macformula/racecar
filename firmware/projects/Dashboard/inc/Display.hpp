#pragma once

#include <optional>

#include "../generated/can/veh_bus.hpp"
#include "../generated/can/veh_messages.hpp"
#include "inc/AcknowledgeConfig.hpp"
#include "inc/Button.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/EventSelect.hpp"
#include "inc/LogoScreen.hpp"
#include "inc/StartDriving.hpp"
#include "inc/StartHV.hpp"
#include "inc/StartMotors.hpp"
#include "inc/StartingHV.hpp"
#include "inc/StartingMotors.hpp"

class Display {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;
    using Driver = generated::can::TxDashboardStatus::Driver_t;
    using Event = generated::can::TxDashboardStatus::Event_t;

    Display(Button& enter, Button& scroll, generated::can::VehBus& veh);

    // CAN and Buttons are public so the ScreenUpdate can access them
    generated::can::VehBus& veh_bus;
    Button enter;
    Button scroll;

    Driver selected_driver = Driver::UNSPECIFIED;
    Event selected_event = Event::UNSPECIFIED;

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
    DriverSelect driver_select;
    EventSelect event_select;
    ConfirmMenu confirm_menu;
    AcknowledgeConfig acknowledge_config;
    StartHV start_hv;
    StartingHV starting_hv;
    StartMotors start_motors;
    StartingMotors starting_motors;
    StartDriving start_driving;
    DriveModeMenu drive_mode;
};