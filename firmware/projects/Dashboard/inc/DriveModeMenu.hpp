#pragma once

#include "Screen.hpp"
#include "lvgl.h"

class DriveModeMenu : public Screen {
public:
    DriveModeMenu(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    const float kArcMaxSpeed = 120;
    const float kArcSpeedResolution = 10;

    lv_obj_t* speedometer_arc;
    lv_obj_t* speed_label;
};
