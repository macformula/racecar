#include "DriveModeMenu.hpp"

#include <src/core/lv_obj.h>
#include <src/core/lv_obj_pos.h>
#include <src/core/lv_obj_style.h>
#include <src/font/lv_font.h>
#include <src/misc/lv_anim.h>
#include <src/misc/lv_area.h>
#include <src/widgets/lv_arc.h>
#include <src/widgets/lv_bar.h>
#include <src/widgets/lv_label.h>

#include "Display.hpp"

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

DriveModeMenu::DriveModeMenu(Display* display) : Screen(display) {}

void DriveModeMenu::CreateGUI() {
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
}
