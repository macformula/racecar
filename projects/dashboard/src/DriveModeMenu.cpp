#include "DriveModeMenu.hpp"

#include "Display.hpp"
#include "lvgl.h"

void Speedometer::Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x,
                       lv_coord_t y) {
    arc = lv_arc_create(parent);
    lv_obj_set_size(arc, 300, 300);
    lv_arc_set_range(arc, 0, kArcMaxSpeed * kArcSpeedResolution);
    lv_obj_align(arc, align, x, y);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

    label = lv_label_create(arc);
    lv_label_set_text(label, "-");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);

    lv_obj_t* unit = lv_label_create(arc);
    lv_label_set_text(unit, "MPH");
    lv_obj_align(unit, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_24, 0);
}

void Speedometer::SetSpeed(float speed) {
    lv_arc_set_value(arc, speed * kArcSpeedResolution);
    lv_label_set_text_fmt(label, "%d", static_cast<int>(speed));
}

// 12.9 16"

void Battery::Draw(lv_obj_t* parent, lv_align_t align, lv_coord_t x,
                   lv_coord_t y) {
    label = lv_label_create(parent);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_label_set_text(label, "--");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);

    lv_obj_t* title = lv_label_create(parent);
    lv_obj_align(title, LV_ALIGN_RIGHT_MID, -120, 50);
    lv_label_set_text(title, "Battery");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);

    SetPercent(0);
}

void Battery::SetPercent(float soc) {
    lv_label_set_text_fmt(label, "%d %%", static_cast<int>(soc));
}

DriveModeMenu::DriveModeMenu(Display* display) 
    : Screen(display), 
      last_warning_time_(0),
      warning_cycle_index_(0) {}

void DriveModeMenu::CreateGUI() {
    // Reset demo state
    last_warning_time_ = lv_tick_get();
    warning_cycle_index_ = 0;
    active_warning_ids_.clear();
    
    // Status bar is always visible (even when empty)
    status_bar_.SetAlwaysVisible(true);
    
    speedometer_.Draw(frame_, LV_ALIGN_LEFT_MID, 100, 0);
    battery_.Draw(frame_, LV_ALIGN_RIGHT_MID, -100, 0);

    lv_obj_t* footer = lv_label_create(frame_);
    lv_label_set_text(footer, "Hold ENTER to shutdown");
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_24, 0);
}

void DriveModeMenu::Update() {
    if (display_->enter.GetHeldDuration() > 3000 &&
        display_->enter.IsPressed()) {
        display_->ChangeState(State::SHUTDOWN);
    }

    auto fc_msg = display_->veh_bus.GetRxDashCommand();

    if (fc_msg.has_value()) {
        speedometer_.SetSpeed(fc_msg->Speed());
        battery_.SetPercent(fc_msg->HvSocPercent());

        if (fc_msg->Errored()) {
            display_->ChangeState(State::ERROR);
        }
    }
    
    // Demo: Cycle warnings every 4 seconds
    uint32_t current_time = lv_tick_get();
    
    // Add a new warning every 4 seconds
    if (current_time - last_warning_time_ >= 1000) {
        // Remove all previous warnings that are 4+ seconds old
        for (int id : active_warning_ids_) {
            status_bar_.RemoveWarning(id);
        }
        active_warning_ids_.clear();
        
        // Add a new warning based on cycle index
        const char* warnings[] = {
            LV_SYMBOL_WARNING,
            "TEMP",
            LV_SYMBOL_CHARGE,
            LV_SYMBOL_POWER,
            "CHECK",
            LV_SYMBOL_STOP
        };
        
        int warning_id = status_bar_.AddWarning(warnings[warning_cycle_index_]);
        active_warning_ids_.push_back(warning_id);
        
        // Cycle to next warning
        warning_cycle_index_ = (warning_cycle_index_ + 1) % 6;
        last_warning_time_ = current_time;
    }
}
