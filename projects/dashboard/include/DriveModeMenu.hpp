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
    void SetPercent(float soc);

private:
    const int kVoltMax = 600;
    const int kVoltMin = 250;

    lv_obj_t* label;
};

class BatteryTemps {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetTemps(float min_temp, float max_temp);

private:
    lv_obj_t* label;
};

class MotorInverterTemps {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetTemps(float motor_temp, float inverter_temp);

private:
    lv_obj_t* label;
};

class HVBatteryVoltageCurrent {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetValues(float voltage, float current);

private:
    lv_obj_t* label;
};

class LVBatteryVoltageCurrent {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetValues(float voltage, float current);

private:
    lv_obj_t* label;
};

class FCStatusMessage {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetStatus(const char* status);

private:
    lv_obj_t* label;
};

class DriveModeMenu : public Screen {
public:
    DriveModeMenu(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    Speedometer speedometer_;
    Battery battery_;
    BatteryTemps battery_temps_;
    MotorInverterTemps motor_inverter_temps_;
    HVBatteryVoltageCurrent hv_batt_vc_;
    LVBatteryVoltageCurrent lv_batt_vc_;
    FCStatusMessage fc_status_;

    uint32_t last_warning_time_;
    etl::vector<int, 10> active_warning_ids_;
    int warning_cycle_index_;
};