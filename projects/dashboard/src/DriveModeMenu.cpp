#include "DriveModeMenu.hpp"

#include "Display.hpp"
#include "lvgl.h"

// ─────────────────────────────────────────────────────────────
// Layout constants & fonts
// ─────────────────────────────────────────────────────────────
// NOTE: enable LV_FONT_MONTSERRAT_20 / _30 / _36 / _40 / _48 in lv_conf.h

namespace {

constexpr lv_coord_t kColTop = 60;
constexpr lv_coord_t kColHeight = 380;
constexpr lv_coord_t kColWidth = 260;

const lv_font_t* kFontCaption = &lv_font_montserrat_20;
const lv_font_t* kFontTitle = &lv_font_montserrat_30;
const lv_font_t* kFontLabel = &lv_font_montserrat_30;
const lv_font_t* kFontMid = &lv_font_montserrat_36;
const lv_font_t* kFontValue = &lv_font_montserrat_40;
const lv_font_t* kFontBig = &lv_font_montserrat_48;

lv_obj_t* MakeColumn(lv_obj_t* parent, lv_coord_t x, const char* name) {
    lv_obj_t* col = lv_obj_create(parent);
    lv_obj_set_size(col, kColWidth, kColHeight);
    lv_obj_align(col, LV_ALIGN_TOP_LEFT, x, kColTop);
    lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(col, 0, 0);
    lv_obj_set_style_pad_all(col, 0, 0);
    lv_obj_set_style_pad_row(col, 6, 0);
    lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(col, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* title = lv_label_create(col);
    lv_obj_set_style_text_font(title, kFontTitle, 0);
    lv_label_set_text(title, name);
    return col;
}

lv_obj_t* MakeRow(lv_obj_t* parent) {
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    return row;
}

}  // namespace

// ─────────────────────────────────────────────────────────────
// HV Arc
// ─────────────────────────────────────────────────────────────

float HVCurrentArc::ComputeAverage() const {
    if (buf_.empty()) return 0.0f;
    float sum = 0;
    for (auto v : buf_) sum += v;
    return sum / buf_.size();
}

void HVCurrentArc::Draw(lv_obj_t* parent) {
    arc_ = lv_arc_create(parent);
    lv_obj_set_size(arc_, 200, 200);
    lv_arc_set_bg_angles(arc_, 135, 45);
    lv_arc_set_range(arc_, 0, (int32_t)(kHVCurrentMax * kArcResolution));
    lv_arc_set_value(arc_, 0);
    lv_obj_remove_style(arc_, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_, 16, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 16, LV_PART_INDICATOR);

    label_ = lv_label_create(arc_);
    lv_obj_set_style_text_font(label_, kFontBig, 0);
    lv_label_set_text(label_, "--");
    lv_obj_align(label_, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t* unit = lv_label_create(arc_);
    lv_obj_set_style_text_font(unit, kFontCaption, 0);
    lv_label_set_text(unit, "HV AMPS");
    lv_obj_align(unit, LV_ALIGN_CENTER, 0, 30);
}

void HVCurrentArc::PushSample(float v) {
    buf_.push(v);
    Update();
}

void HVCurrentArc::Update() {
    float avg = ComputeAverage();
    lv_arc_set_value(arc_, (int32_t)(avg * kArcResolution));
    lv_label_set_text_fmt(label_, "%d", (int)avg);
}

// ─────────────────────────────────────────────────────────────
// Voltage / Current
// ─────────────────────────────────────────────────────────────

void HVVoltageDisplay::Draw(lv_obj_t* parent) {
    label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(label_, kFontValue, 0);
    lv_label_set_text(label_, "HV -- V");
}

void HVVoltageDisplay::SetVoltage(float v) {
    lv_label_set_text_fmt(label_, "HV %d V", (int)v);
}

void LVVoltageDisplay::Draw(lv_obj_t* parent) {
    label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(label_, kFontValue, 0);
    lv_label_set_text(label_, "LV -- V");
}

void LVVoltageDisplay::SetVoltage(float v) {
    lv_label_set_text_fmt(label_, "LV %d V", (int)v);
}

void LVCurrentDisplay::Draw(lv_obj_t* parent) {
    label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(label_, kFontValue, 0);
    lv_label_set_text(label_, "LV -- A");
}

void LVCurrentDisplay::SetCurrent(float v) {
    lv_label_set_text_fmt(label_, "LV %d A", (int)v);
}

// ─────────────────────────────────────────────────────────────
// Battery SOC
// ─────────────────────────────────────────────────────────────

void Battery::Draw(lv_obj_t* parent) {
    lv_obj_t* wrap = lv_obj_create(parent);
    lv_obj_set_size(wrap, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(wrap, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wrap, 0, 0);
    lv_obj_set_style_pad_all(wrap, 0, 0);
    lv_obj_set_style_pad_row(wrap, 4, 0);
    lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(wrap, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(wrap, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    bar_ = lv_bar_create(wrap);
    lv_bar_set_range(bar_, 0, 100);
    lv_obj_set_size(bar_, LV_PCT(100), 26);

    label_ = lv_label_create(wrap);
    lv_obj_set_style_text_font(label_, kFontValue, 0);
    lv_label_set_text(label_, "--%");
}

void Battery::SetSOC(float soc) {
    lv_bar_set_value(bar_, (int)soc, LV_ANIM_OFF);
    lv_label_set_text_fmt(label_, "%d%%", (int)soc);
}

// ─────────────────────────────────────────────────────────────
// Temp Card
// ─────────────────────────────────────────────────────────────

void TempCard::Draw(lv_obj_t* parent, const char* title, lv_color_t color) {
    container_ = lv_obj_create(parent);
    lv_obj_set_width(container_, LV_PCT(100));
    lv_obj_set_flex_grow(container_, 1);

    lv_obj_set_style_bg_color(container_, color, 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(container_, 8, 0);
    lv_obj_set_style_pad_all(container_, 8, 0);
    lv_obj_set_style_pad_row(container_, 2, 0);
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t* t = lv_label_create(container_);
    lv_obj_set_style_text_font(t, kFontCaption, 0);
    lv_label_set_text(t, title);

    lv_obj_t* row = MakeRow(container_);
    min_label_ = lv_label_create(row);
    lv_obj_set_style_text_font(min_label_, kFontMid, 0);
    lv_label_set_text(min_label_, "Min --");

    max_label_ = lv_label_create(row);
    lv_obj_set_style_text_font(max_label_, kFontMid, 0);
    lv_label_set_text(max_label_, "Max --");
}

void TempCard::SetTemps(float min, float max) {
    lv_label_set_text_fmt(min_label_, "Min: %d C", (int)min);
    lv_label_set_text_fmt(max_label_, "Max: %d C", (int)max);
}

// ─────────────────────────────────────────────────────────────
// CAN Indicator
// ─────────────────────────────────────────────────────────────

void CANIndicator::Draw(lv_obj_t* parent, const char* label) {
    lv_obj_t* wrap = lv_obj_create(parent);
    lv_obj_set_size(wrap, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(wrap, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wrap, 0, 0);
    lv_obj_set_style_pad_all(wrap, 0, 0);
    lv_obj_set_style_pad_column(wrap, 8, 0);
    lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(wrap, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(wrap, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    dot_ = lv_obj_create(wrap);
    lv_obj_set_size(dot_, 18, 18);
    lv_obj_set_style_radius(dot_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot_, 0, 0);
    lv_obj_set_style_bg_color(dot_, lv_color_hex(0x444444), 0);

    label_ = lv_label_create(wrap);
    lv_obj_set_style_text_font(label_, kFontLabel, 0);
    lv_label_set_text(label_, label);
}

void CANIndicator::SetActive(bool active) {
    lv_obj_set_style_bg_color(
        dot_, active ? lv_color_hex(0x00FF00) : lv_color_hex(0x444444), 0);
}

// ─────────────────────────────────────────────────────────────
// FC Status
// ─────────────────────────────────────────────────────────────

void FCStatusMessage::Draw(lv_obj_t* parent) {
    label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(label_, kFontLabel, 0);
    lv_label_set_text(label_, "FC: --");
}

void FCStatusMessage::SetStatus(const char* s) {
    lv_label_set_text_fmt(label_, "FC: %s", s);
}

// ─────────────────────────────────────────────────────────────
// Speedometer (compact, bottom of right column)
// ─────────────────────────────────────────────────────────────

void Speedometer::Draw(lv_obj_t* parent) {
    arc_ = lv_arc_create(parent);
    lv_obj_set_size(arc_, 130, 130);
    lv_arc_set_bg_angles(arc_, 135, 45);
    lv_arc_set_range(arc_, 0, (int32_t)kSpeedMax);
    lv_arc_set_value(arc_, 0);
    lv_obj_remove_style(arc_, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_, 10, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 10, LV_PART_INDICATOR);

    value_label_ = lv_label_create(arc_);
    lv_obj_set_style_text_font(value_label_, kFontMid, 0);
    lv_label_set_text(value_label_, "--");
    lv_obj_align(value_label_, LV_ALIGN_CENTER, 0, -8);

    lv_obj_t* unit = lv_label_create(arc_);
    lv_obj_set_style_text_font(unit, kFontCaption, 0);
    lv_label_set_text(unit, "km/h");
    lv_obj_align(unit, LV_ALIGN_CENTER, 0, 22);
}

void Speedometer::SetSpeed(float kph) {
    if (kph < 0.0f) kph = 0.0f;
    if (kph > kSpeedMax) kph = kSpeedMax;
    lv_arc_set_value(arc_, (int32_t)kph);
    lv_label_set_text_fmt(value_label_, "%d", (int)kph);
}

// ─────────────────────────────────────────────────────────────
// DriveModeMenu
// ─────────────────────────────────────────────────────────────

DriveModeMenu::DriveModeMenu(Display* display) : Screen(display) {}

void DriveModeMenu::CreateGUI() {
    lv_obj_t* left = MakeColumn(frame_, 10, "HV");
    hv_arc_.Draw(left);      // 200 px arc, 48 pt center value
    hv_voltage_.Draw(left);  // 40 pt
    battery_.Draw(left);     // full-width bar + 40 pt SOC

    lv_obj_t* mid = MakeColumn(frame_, 270, "THERMALS");
    battery_temp_.Draw(mid, "BATTERY", lv_color_hex(0xAA0000));
    motor_temp_.Draw(mid, "MOTOR", lv_color_hex(0x00AA00));
    inverter_temp_.Draw(mid, "INVERTER", lv_color_hex(0x0000AA));

    lv_obj_t* right = MakeColumn(frame_, 530, "LV");
    lv_voltage_.Draw(right);
    lv_current_.Draw(right);
    fc_status_.Draw(right);

    lv_obj_t* can_row = MakeRow(right);
    pt_can_.Draw(can_row, "PT");
    veh_can_.Draw(can_row, "VEH");

    speedo_.Draw(right);
}

void DriveModeMenu::Update() {
    auto lv_msg = display_->veh_bus.GetRxLvDcdc();
    auto acc_msg = display_->veh_bus.GetRxAccumulator_Soc();
    auto bms_msg = display_->veh_bus.GetRxBmsBroadcast();

    hv_arc_.PushSample(0.0f);

    lv_current_.SetCurrent(lv_msg ? lv_msg->BusCurrent() : 0.0f);

    lv_voltage_.SetVoltage(lv_msg ? lv_msg->LvBatteryVoltage() : 0.0f);
    hv_voltage_.SetVoltage(acc_msg ? acc_msg->PackVoltage() : 0.0f);

    battery_.SetSOC(acc_msg ? acc_msg->SocPercent() : 0.0f);

    if (bms_msg) {
        battery_temp_.SetTemps(bms_msg->LowThermValue(),
                               bms_msg->HighThermValue());
    }

    motor_temp_.SetTemps(0.0f, 0.0f);
    inverter_temp_.SetTemps(0.0f, 0.0f);

    fc_status_.SetStatus("OK");

    speedo_.SetSpeed(0.0f);  // TODO: wire to wheel speed / inverter RPM msg

    pt_can_.SetActive(false);
    veh_can_.SetActive(true);
}