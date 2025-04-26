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
#include "lvgl/lvgl.h"

class Menu {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;
    using Driver = generated::can::TxDashboardStatus::Driver_t;
    using Event = generated::can::TxDashboardStatus::Event_t;

    Menu(generated::can::VehBus& veh);

    generated::can::VehBus& veh_bus;

    // holds the selected driver and mode that are updated within DriverSelect
    // and ModeSelect respectively
    Driver selected_driver = Menu::Driver::UNSPECIFIED;
    Event selected_event = Menu::Event::UNSPECIFIED;
    State state_ = Menu::State::LOGO;

    Screen* screen_;

    void ChangeState(State screen);

private:
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