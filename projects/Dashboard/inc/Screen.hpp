#pragma once

#include "generated/can/veh_messages.hpp"
#include "lvgl.h"

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

private:
    virtual void CreateGUI() = 0;
};