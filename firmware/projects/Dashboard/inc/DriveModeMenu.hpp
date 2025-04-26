#pragma once

#include "Screen.hpp"
#include "lvgl/lvgl.h"

class DriveModeMenu : public Screen {
public:
    DriveModeMenu(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;

private:
    int speed;
    bool increasing;
    lv_timer_t* speed_timer;
    void speed_update_cb(lv_timer_t* timer);
};
