#pragma once

#include "Screen.hpp"
#include "etl/circular_buffer.h"
#include "etl/vector.h"
#include "lvgl.h"

// ── HV current arc ───────────────────────────────────────────────────────────
class HVCurrentArc {
public:
    static constexpr uint32_t kUpdateRateHz = 100;
    static constexpr uint32_t kAvgWindowSamples = kUpdateRateHz;
    static constexpr float kHVCurrentMax = 500.0f;
    static constexpr float kArcResolution = 10.0f;

    void Draw(lv_obj_t* parent);
    void PushSample(float hv_current);

private:
    lv_obj_t* arc_{nullptr};
    lv_obj_t* label_{nullptr};
    etl::circular_buffer<float, kAvgWindowSamples> buf_;

    float ComputeAverage() const;
    void Update();
};

// ── HV / LV voltage ──────────────────────────────────────────────────────────
class HVVoltageDisplay {
public:
    void Draw(lv_obj_t* parent);
    void SetVoltage(float hv_voltage);

private:
    lv_obj_t* label_{nullptr};
};

class LVVoltageDisplay {
public:
    void Draw(lv_obj_t* parent);
    void SetVoltage(float lv_voltage);

private:
    lv_obj_t* label_{nullptr};
};

// ── LV current (text) ────────────────────────────────────────────────────────
class LVCurrentDisplay {
public:
    void Draw(lv_obj_t* parent);
    void SetCurrent(float current);

private:
    lv_obj_t* label_{nullptr};
};

// ── SOC ──────────────────────────────────────────────────────────────────────
class Battery {
public:
    void Draw(lv_obj_t* parent);
    void SetSOC(float soc);

private:
    lv_obj_t* bar_{nullptr};
    lv_obj_t* label_{nullptr};
};

// ── Temperature card ─────────────────────────────────────────────────────────
class TempCard {
public:
    void Draw(lv_obj_t* parent, const char* title, lv_color_t color);
    void SetTemps(float min, float max);

private:
    lv_obj_t* container_{nullptr};
    lv_obj_t* min_label_{nullptr};
    lv_obj_t* max_label_{nullptr};
};

// ── CAN indicator ────────────────────────────────────────────────────────────
class CANIndicator {
public:
    void Draw(lv_obj_t* parent, const char* label);
    void SetActive(bool active);

private:
    lv_obj_t* dot_{nullptr};
    lv_obj_t* label_{nullptr};
};

// ── FC status ────────────────────────────────────────────────────────────────
class FCStatusMessage {
public:
    void Draw(lv_obj_t* parent);
    void SetStatus(const char* status);

private:
    lv_obj_t* label_{nullptr};
};

// ── Speedometer (compact) ────────────────────────────────────────────────────
class Speedometer {
public:
    static constexpr float kSpeedMax = 140.0f;  // km/h, FSAE-realistic ceiling

    void Draw(lv_obj_t* parent);
    void SetSpeed(float speed_kph);

private:
    lv_obj_t* arc_{nullptr};
    lv_obj_t* value_label_{nullptr};
};

// ── Screen ───────────────────────────────────────────────────────────────────
class DriveModeMenu : public Screen {
public:
    explicit DriveModeMenu(Display* display);
    void CreateGUI() override;
    void Update() override;

private:
    HVCurrentArc hv_arc_;
    HVVoltageDisplay hv_voltage_;
    LVVoltageDisplay lv_voltage_;
    LVCurrentDisplay lv_current_;
    Battery battery_;

    TempCard battery_temp_;
    TempCard motor_temp_;
    TempCard inverter_temp_;

    CANIndicator pt_can_;
    CANIndicator veh_can_;

    FCStatusMessage fc_status_;
    Speedometer speedo_;

    uint32_t last_warning_time_{0};
    etl::vector<int, 10> active_warning_ids_;
    int warning_cycle_index_{0};
};