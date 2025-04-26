#pragma once

#include "generated/can/veh_messages.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class Menu;

class Screen {
public:
    using State = generated::can::TxDashboardStatus::DashState_t;

    Screen(Menu* menu) : menu_(menu) {}

    void Create();
    virtual void Update(Button select, Button scroll) = 0;

protected:
    Menu* menu_;
    lv_obj_t* frame;

private:
    virtual void PostCreate() = 0;
};