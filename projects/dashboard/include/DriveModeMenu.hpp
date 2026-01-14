#pragma once

#include "Screen.hpp"
#include "etl/vector.h"
#include "lvgl.h"

class Speedometer {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetSpeed(float speed);

private:
    const float kArcMaxSpeed = 120;
    const float kArcSpeedResolution = 10;

    lv_obj_t* arc;
    lv_obj_t* label;
};

class Battery {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetPercent(float volt);

private:
    const int kVoltMax = 600.f;
    const int kVoltMin = 250.f;  // display this as "empty"

    lv_obj_t* label;
};

class DriveModeMenu : public Screen {
public:
    DriveModeMenu(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    Battery battery_;
    Speedometer speedometer_;

    // Demo: cycling warnings
    uint32_t last_warning_time_;
    etl::vector<int, 10> active_warning_ids_;
    int warning_cycle_index_;
};
