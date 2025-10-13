#include "StatusBar.hpp"

StatusBar::StatusBar()
    : bar_container_(nullptr),
      icon_container_(nullptr),
      next_warning_id_(1),
      always_visible_(true) {}  // Default: always visible

void StatusBar::Create(lv_obj_t* parent) {
    // Reset state in case Create() is called multiple times
    warnings_.clear();
    bar_container_ = nullptr;
    icon_container_ = nullptr;

    // Create the red/orange bar container at the top
    bar_container_ = lv_obj_create(parent);
    lv_obj_set_size(bar_container_, LV_PCT(100), 50);  // Full width, 50px tall
    lv_obj_align(bar_container_, LV_ALIGN_TOP_MID, 0, 0);

    // Style: Red/orange background
    lv_obj_set_style_bg_color(bar_container_, lv_color_hex(0xFF4500),
                              LV_PART_MAIN);  // Orange-red
    lv_obj_set_style_bg_opa(bar_container_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(bar_container_, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bar_container_, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(bar_container_, 5, LV_PART_MAIN);

    // Create a flex container for icons (horizontal layout)
    icon_container_ = lv_obj_create(bar_container_);
    lv_obj_set_size(icon_container_, LV_PCT(100), LV_PCT(100));
    lv_obj_center(icon_container_);
    lv_obj_set_flex_flow(icon_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_container_, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(icon_container_, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(icon_container_, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(icon_container_, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_all(icon_container_, 5, LV_PART_MAIN);

    // Visibility based on always_visible_ setting
    UpdateVisibility();
}

int StatusBar::AddWarning(const char* icon_text) {
    if (!icon_container_) return -1;

    // Create warning icon label
    lv_obj_t* icon_label = lv_label_create(icon_container_);
    lv_label_set_text(icon_label, icon_text);
    lv_obj_set_style_text_color(icon_label, lv_color_hex(0xFFFFFF),
                                LV_PART_MAIN);  // White text
    lv_obj_set_style_text_font(icon_label, &lv_font_montserrat_24,
                               LV_PART_MAIN);

    // Store the warning
    Warning warning;
    warning.id = next_warning_id_++;
    warning.label = icon_label;
    warning.text = icon_text;
    warnings_.push_back(warning);

    // Update visibility
    UpdateVisibility();

    return warning.id;
}

void StatusBar::RemoveWarning(int warning_id) {
    for (auto it = warnings_.begin(); it != warnings_.end(); ++it) {
        if (it->id == warning_id) {
            // Delete the LVGL object
            lv_obj_delete(it->label);

            // Remove from vector
            warnings_.erase(it);

            // Update visibility
            UpdateVisibility();

            return;
        }
    }
}

void StatusBar::ClearAllWarnings() {
    // Delete all warning labels
    for (auto& warning : warnings_) {
        lv_obj_delete(warning.label);
    }

    // Clear the vector
    warnings_.clear();

    // Update visibility
    UpdateVisibility();
}

bool StatusBar::IsVisible() const {
    if (!bar_container_) return false;
    return !lv_obj_has_flag(bar_container_, LV_OBJ_FLAG_HIDDEN);
}

void StatusBar::SetVisible(bool visible) {
    if (!bar_container_) return;

    if (visible) {
        lv_obj_clear_flag(bar_container_, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(bar_container_, LV_OBJ_FLAG_HIDDEN);
    }
}

void StatusBar::SetAlwaysVisible(bool always_visible) {
    always_visible_ = always_visible;
    UpdateVisibility();
}

void StatusBar::UpdateVisibility() {
    if (!bar_container_) return;

    // Show if: always_visible OR has warnings
    bool should_show = always_visible_ || !warnings_.empty();

    if (should_show) {
        lv_obj_clear_flag(bar_container_, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(bar_container_, LV_OBJ_FLAG_HIDDEN);
    }
}

void StatusBar::UpdateLayout() {
    // Currently layout is automatic via flex
    // This method can be used for future customization
}
