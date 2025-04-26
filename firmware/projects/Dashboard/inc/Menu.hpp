#pragma once

#include "../generated/can/veh_bus.hpp"
#include "../generated/can/veh_messages.hpp"
#include "inc/ConfirmMenu.hpp"
#include "inc/DriveModeMenu.hpp"
#include "inc/DriverSelect.hpp"
#include "inc/EventSelect.hpp"
#include "inc/LogoScreen.hpp"
#include "inc/StartDriving.hpp"
#include "inc/StartHV.hpp"
#include "inc/StartMotors.hpp"
#include "inc/WaitingScreen.hpp"

class Menu {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;
    using Driver = generated::can::TxDashboardStatus::Driver_t;
    using Event = generated::can::TxDashboardStatus::Event_t;

    Menu(generated::can::VehBus& veh);

    generated::can::VehBus& veh_bus;

    Driver selected_driver = Driver::UNSPECIFIED;
    Event selected_event = Event::UNSPECIFIED;

    Screen* screen_;

    void ChangeState(State screen);
    State GetState() const;

private:
    State state_ = State::LOGO;

    LogoScreen logo_screen;
    DriverSelect driver_select;
    EventSelect event_select;
    ConfirmMenu confirm_menu;
    StartHV start_hv;
    WaitingScreen waiting_screen;
    StartMotors start_motors;
    StartDriving start_driving;
    DriveModeMenu drive_mode;
};