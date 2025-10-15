#pragma once

#include <string>

#include "etl/vector.h"
#include "lvgl.h"

/**
 * StatusBar - A persistent debug/warning bar that appears on all screens.
 *
 * This bar displays warning icons (like check engine lights) at the top of the
 * screen and is completely independent of individual screen implementations.
 */
class StatusBar {
public:
    StatusBar();

    /**
     * Create the status bar UI elements.
     * Should be called once when initializing a screen.
     *
     * @param parent The parent LVGL object to attach the status bar to
     */
    void Create(lv_obj_t* parent);

    /**
     * Add a warning icon to the status bar.
     * Icons appear left-to-right in the order they're added.
     *
     * @param icon_text LVGL symbol (e.g., LV_SYMBOL_WARNING) or text/emoji
     * @return Unique ID for this warning (use to remove it later)
     */
    int AddWarning(const char* icon_text);

    /**
     * Remove a warning icon by its ID.
     *
     * @param warning_id The ID returned from AddWarning()
     */
    void RemoveWarning(int warning_id);

    /**
     * Remove all warnings from the status bar.
     */
    void ClearAllWarnings();

private:
    lv_obj_t* bar_container_;   // The red/orange background bar
    lv_obj_t* icon_container_;  // Container for warning icons

    struct Warning {
        int id;
        lv_obj_t* label;
        std::string text;
    };

    etl::vector<Warning, 20> warnings_;
    int next_warning_id_;
};
