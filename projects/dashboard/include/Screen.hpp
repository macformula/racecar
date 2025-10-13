#pragma once

#include "generated/can/veh_messages.hpp"
#include "lvgl.h"
#include "StatusBar.hpp"

class Display;

class Screen {
public:
    using State = generated::can::TxDashStatus::State_t;

    Screen(Display* display);

    void Create();
    virtual void Update() = 0;

protected:
    Display* const display_;
    lv_obj_t* frame_;
    StatusBar status_bar_;  // Persistent warning/debug bar on all screens

private:
    virtual void CreateGUI() = 0;
};