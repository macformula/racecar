#pragma once

#include "Screen.hpp"
#include "etl/circular_buffer.h"
#include "etl/vector.h"
#include "lvgl.h"

class CurrentArcs {
public:
    static constexpr uint32_t kUpdateRateHz = 100;
    static constexpr uint32_t kAvgWindowSamples = kUpdateRateHz;  // 1s window
    static constexpr float kHVCurrentMax = 500.0f;                // A — tune
    static constexpr float kLVCurrentMax = 50.0f;                 // A — tune
    static constexpr float kArcResolution = 10.0f;  // ticks per amp

    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);

    // Push one sample per Update() tick.
    void PushSamples(float hv_current, float lv_current);

private:
    lv_obj_t* hv_arc_{nullptr};
    lv_obj_t* lv_arc_{nullptr};
    lv_obj_t* hv_label_{nullptr};
    lv_obj_t* lv_label_{nullptr};

    etl::circular_buffer<float, kAvgWindowSamples> hv_buf_;
    etl::circular_buffer<float, kAvgWindowSamples> lv_buf_;

    float ComputeAverage(
        const etl::circular_buffer<float, kAvgWindowSamples>& buf) const;
    void UpdateArcs();
};

class VoltageDisplay {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetVoltages(float hv_voltage, float lv_voltage);

private:
    lv_obj_t* container_{nullptr};
    lv_obj_t* hv_label_{nullptr};
    lv_obj_t* lv_label_{nullptr};
};

class Battery {
public:
    void Draw(lv_obj_t* parent);
    void SetSOC(float soc);

private:
    lv_obj_t* row_{nullptr};
    lv_obj_t* soc_bar_{nullptr};
    lv_obj_t* soc_label_{nullptr};
};
class BatteryTemps {
public:
    void Draw(lv_obj_t* parent);
    void SetTemps(float min_temp, float max_temp);

private:
    lv_obj_t* container_{nullptr};
    lv_obj_t* min_label_{nullptr};
    lv_obj_t* max_label_{nullptr};
};

class MotorInverterTemps {
public:
    void Draw(lv_obj_t* parent);
    void SetTemps(float motor_temp, float inverter_temp);

private:
    lv_obj_t* container_{nullptr};
    lv_obj_t* motor_label_{nullptr};
    lv_obj_t* inv_label_{nullptr};
};

class FCStatusMessage {
public:
    void Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x, lv_coord_t y);
    void SetStatus(const char* status);

private:
    lv_obj_t* label{nullptr};
};

class DriveModeMenu : public Screen {
public:
    explicit DriveModeMenu(Display* display);
    void CreateGUI() override;
    void Update() override;

private:
    CurrentArcs current_arcs_;
    VoltageDisplay voltage_display_;
    Battery battery_;
    BatteryTemps battery_temps_;
    MotorInverterTemps motor_inverter_temps_;
    FCStatusMessage fc_status_;

    uint32_t last_warning_time_{0};
    etl::vector<int, 10> active_warning_ids_;
    int warning_cycle_index_{0};
};