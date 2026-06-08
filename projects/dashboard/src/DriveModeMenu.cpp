#include "DriveModeMenu.hpp"

#include "Display.hpp"
#include "lvgl.h"

float CurrentArcs::ComputeAverage(
    const etl::circular_buffer<float, kAvgWindowSamples>& buf) const {
    if (buf.empty()) return 0.0f;
    float sum = 0.0f;
    for (const float v : buf) sum += v;
    return sum / static_cast<float>(buf.size());
}

void CurrentArcs::Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x,
                       lv_coord_t y) {
    hv_arc_ = lv_arc_create(parent);
    lv_obj_set_size(hv_arc_, 300, 300);
    lv_arc_set_range(hv_arc_, 0,
                     static_cast<int32_t>(kHVCurrentMax * kArcResolution));
    lv_arc_set_value(hv_arc_, 0);
    lv_obj_align(hv_arc_, align, x, y);
    lv_obj_remove_style(hv_arc_, NULL, LV_PART_KNOB);

    lv_arc_ = lv_arc_create(parent);
    lv_obj_set_size(lv_arc_, 180, 180);
    lv_arc_set_range(lv_arc_, 0,
                     static_cast<int32_t>(kLVCurrentMax * kArcResolution));
    lv_arc_set_value(lv_arc_, 0);
    lv_obj_align_to(lv_arc_, hv_arc_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_remove_style(lv_arc_, NULL, LV_PART_KNOB);

    hv_label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(hv_label_, &lv_font_montserrat_20, 0);
    lv_label_set_text(hv_label_, "HV: -- A");
    lv_obj_align_to(hv_label_, hv_arc_, LV_ALIGN_CENTER, 0, -18);

    lv_label_ = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_label_, &lv_font_montserrat_20, 0);
    lv_label_set_text(lv_label_, "LV: -- A");
    lv_obj_align_to(lv_label_, hv_arc_, LV_ALIGN_CENTER, 0, 18);

    lv_obj_t* desc_label = lv_label_create(parent);
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_14, 0);
    lv_label_set_text(desc_label, "HV / LV Current");
    lv_obj_align_to(desc_label, hv_arc_, LV_ALIGN_BOTTOM_MID, 0, -4);
}

void CurrentArcs::PushSamples(float hv_current, float lv_current) {
    hv_buf_.push(hv_current);
    lv_buf_.push(lv_current);
    UpdateArcs();
}

void CurrentArcs::UpdateArcs() {
    const float hv_avg = ComputeAverage(hv_buf_);
    const float lv_avg = ComputeAverage(lv_buf_);

    if (hv_arc_) {
        lv_arc_set_value(hv_arc_,
                         static_cast<int32_t>(hv_avg * kArcResolution));
        lv_label_set_text_fmt(hv_label_, "HV: %d A", static_cast<int>(hv_avg));
    }
    if (lv_arc_) {
        lv_arc_set_value(lv_arc_,
                         static_cast<int32_t>(lv_avg * kArcResolution));
        lv_label_set_text_fmt(lv_label_, "LV: %d A", static_cast<int>(lv_avg));
    }
}

void VoltageDisplay::Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x,
                          lv_coord_t y) {
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, 200, LV_SIZE_CONTENT);
    lv_obj_align(container_, align, x, y);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 8, 0);

    lv_obj_set_layout(container_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* title = lv_label_create(container_);
    lv_label_set_text(title, "Voltage");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    hv_label_ = lv_label_create(container_);
    lv_label_set_text(hv_label_, "HV: --- V");
    lv_obj_set_style_text_font(hv_label_, &lv_font_montserrat_36, 0);

    lv_label_ = lv_label_create(container_);
    lv_label_set_text(lv_label_, "LV:  -- V");
    lv_obj_set_style_text_font(lv_label_, &lv_font_montserrat_36, 0);
}

void VoltageDisplay::SetVoltages(float hv_voltage, float lv_voltage) {
    if (hv_label_)
        lv_label_set_text_fmt(hv_label_, "HV: %d V",
                              static_cast<int>(hv_voltage));
    if (lv_label_)
        lv_label_set_text_fmt(lv_label_, "LV: %d V",
                              static_cast<int>(lv_voltage));
}

void Battery::Draw(lv_obj_t* parent) {
    lv_obj_t* wrapper = lv_obj_create(parent);
    lv_obj_set_size(wrapper, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(wrapper, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wrapper, 0, 0);
    lv_obj_set_style_pad_all(wrapper, 0, 0);
    lv_obj_set_style_pad_row(wrapper, 6, 0);

    lv_obj_set_layout(wrapper, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(wrapper, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(wrapper, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_END);

    lv_obj_t* title = lv_label_create(wrapper);
    lv_label_set_text(title, "Battery");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    row_ = lv_obj_create(wrapper);
    lv_obj_set_size(row_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row_, 0, 0);
    lv_obj_set_style_pad_all(row_, 0, 0);
    lv_obj_set_style_pad_column(row_, 10, 0);

    lv_obj_set_layout(row_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    soc_bar_ = lv_bar_create(row_);
    lv_obj_set_size(soc_bar_, 20, 120);
    lv_bar_set_range(soc_bar_, 0, 100);
    lv_bar_set_value(soc_bar_, 0, LV_ANIM_OFF);
    lv_bar_set_orientation(soc_bar_, LV_BAR_ORIENTATION_VERTICAL);

    soc_label_ = lv_label_create(row_);
    lv_label_set_text(soc_label_, "--%");
    lv_obj_set_style_text_font(soc_label_, &lv_font_montserrat_48, 0);
}

void Battery::SetSOC(float soc) {
    if (soc_label_)
        lv_label_set_text_fmt(soc_label_, "%d%%", static_cast<int>(soc));
    if (soc_bar_)
        lv_bar_set_value(soc_bar_, static_cast<int32_t>(soc), LV_ANIM_ON);
}

void BatteryTemps::Draw(lv_obj_t* parent) {
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 4, 0);

    lv_obj_set_layout(container_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_END);

    lv_obj_t* title = lv_label_create(container_);
    lv_label_set_text(title, "Cell temps");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    min_label_ = lv_label_create(container_);
    lv_label_set_text(min_label_, "Min: -- C");
    lv_obj_set_style_text_font(min_label_, &lv_font_montserrat_24, 0);

    max_label_ = lv_label_create(container_);
    lv_label_set_text(max_label_, "Max: -- C");
    lv_obj_set_style_text_font(max_label_, &lv_font_montserrat_24, 0);
}

void BatteryTemps::SetTemps(float min_temp, float max_temp) {
    if (min_label_)
        lv_label_set_text_fmt(min_label_, "Min: %d C",
                              static_cast<int>(min_temp));
    if (max_label_)
        lv_label_set_text_fmt(max_label_, "Max: %d C",
                              static_cast<int>(max_temp));
}

void MotorInverterTemps::Draw(lv_obj_t* parent) {
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 4, 0);

    lv_obj_set_layout(container_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_END);

    lv_obj_t* title = lv_label_create(container_);
    lv_label_set_text(title, "Drive temps");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    motor_label_ = lv_label_create(container_);
    lv_label_set_text(motor_label_, "Motor: -- C");
    lv_obj_set_style_text_font(motor_label_, &lv_font_montserrat_24, 0);

    inv_label_ = lv_label_create(container_);
    lv_label_set_text(inv_label_, "Inv:   -- C");
    lv_obj_set_style_text_font(inv_label_, &lv_font_montserrat_24, 0);
}

void MotorInverterTemps::SetTemps(float motor_temp, float inverter_temp) {
    if (motor_label_)
        lv_label_set_text_fmt(motor_label_, "Motor: %d C",
                              static_cast<int>(motor_temp));
    if (inv_label_)
        lv_label_set_text_fmt(inv_label_, "Inv:   %d C",
                              static_cast<int>(inverter_temp));
}

void FCStatusMessage::Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x,
                           lv_coord_t y) {
    label = lv_label_create(parent);
    lv_obj_align(label, align, x, y);
    lv_label_set_text(label, "FC: --");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
}

void FCStatusMessage::SetStatus(const char* status) {
    lv_label_set_text_fmt(label, "FC: %s", status);
}

DriveModeMenu::DriveModeMenu(Display* display) : Screen(display) {}

void DriveModeMenu::CreateGUI() {
    last_warning_time_ = lv_tick_get();
    warning_cycle_index_ = 0;
    active_warning_ids_.clear();

    current_arcs_.Draw(frame_, LV_ALIGN_TOP_LEFT, 10, 90);

    voltage_display_.Draw(frame_, LV_ALIGN_TOP_MID, 60, 80);

    lv_obj_t* right_col = lv_obj_create(frame_);
    lv_obj_set_size(right_col, 180, 410);

    lv_obj_align(right_col, LV_ALIGN_TOP_RIGHT, -10, 50);
    lv_obj_set_style_bg_opa(right_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_col, 0, 0);
    lv_obj_set_style_pad_all(right_col, 0, 0);
    lv_obj_set_style_pad_row(right_col, 0, 0);

    lv_obj_set_layout(right_col, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(right_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(right_col, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    battery_.Draw(right_col);
    battery_temps_.Draw(right_col);
    motor_inverter_temps_.Draw(right_col);

    fc_status_.Draw(frame_, LV_ALIGN_BOTTOM_MID, 0, -35);

    lv_obj_t* footer = lv_label_create(frame_);
    lv_label_set_text(footer, "Hold ENTER to shutdown");
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_14, 0);
}

void DriveModeMenu::Update() {
    if (display_->enter.GetHeldDuration() > 3000 &&
        display_->enter.IsPressed()) {
        display_->ChangeState(State::SHUTDOWN);
    }

    auto fc_msg = display_->veh_bus.GetRxDashCommand();
    auto lv_msg = display_->veh_bus.GetRxLvDcdc();
    auto acc_msg = display_->veh_bus.GetRxAccumulator_Soc();
    auto bms_msg = display_->veh_bus.GetRxBmsBroadcast();

    if (fc_msg.has_value()) {
        const float hv_current = 120.0f;  //! no HVCurrent signal exists yet
        const float lv_current =
            lv_msg.has_value() ? lv_msg->BusCurrent() : 0.0f;
        current_arcs_.PushSamples(hv_current, lv_current);

        voltage_display_.SetVoltages(
            acc_msg.has_value() ? acc_msg->PackVoltage() : 0.0f,
            lv_msg.has_value() ? lv_msg->LvBatteryVoltage() : 0.0f);

        battery_.SetSOC(acc_msg.has_value()
                            ? static_cast<float>(acc_msg->SocPercent())
                            : 0.0f);

        battery_temps_.SetTemps(
            bms_msg.has_value() ? bms_msg->LowThermValue() : 0.0f,
            bms_msg.has_value() ? bms_msg->HighThermValue() : 0.0f);

        motor_inverter_temps_.SetTemps(
            67.0f, 67.0f);  //! no MotorTemp/InverterTemp yet

        fc_status_.SetStatus("Hi");  //! no FC status

        if (fc_msg->Errored()) {
            display_->ChangeState(State::ERROR);
        }
    }
    // ... warning cycling unchanged
}