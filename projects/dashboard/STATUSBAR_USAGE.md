# StatusBar Usage Guide

The StatusBar is a persistent red/orange warning bar that appears at the top of **ALL screens** automatically. It's perfect for showing "check engine light" style warnings to the driver.

## Features

- Red/orange background (`#FF4500` - OrangeRed)
- Appears on all screens automatically
- Completely separate from screen logic
- Auto-hides when empty, auto-shows when warnings are added
- 50px tall bar at the top of the screen

## How to Use

Every screen inherits `status_bar_` from the base `Screen` class, so you can access it in any screen's `CreateGUI()` or `Update()` method.

### Example 1: Add a Warning Icon

```cpp
void MyScreen::Update() {
    // Add a warning using LVGL symbols
    int warning_id = status_bar_.AddWarning(LV_SYMBOL_WARNING);
    
    // Or use text
    int temp_warning = status_bar_.AddWarning("TEMP!");
}
```

### Example 2: Add and Remove Warnings Based on Conditions

```cpp
void DriveModeMenu::Update() {
    auto veh_msg = display_->veh_bus.GetRxVehicleStatus();
    
    if (veh_msg.has_value()) {
        // Check for high temperature
        if (veh_msg->MotorTemp() > 80) {
            // Store the warning ID if you want to remove it later
            temp_warning_id_ = status_bar_.AddWarning(LV_SYMBOL_WARNING " HOT");
        } else {
            // Remove the warning when condition clears
            status_bar_.RemoveWarning(temp_warning_id_);
        }
    }
}
```

### Example 3: Multiple Warnings

```cpp
void ErrorScreen::CreateGUI() {
    // Add multiple warnings - they appear left to right
    status_bar_.AddWarning(LV_SYMBOL_WARNING);
    status_bar_.AddWarning(LV_SYMBOL_STOP);
    status_bar_.AddWarning("ERROR");
}
```

### Example 4: Clear All Warnings

```cpp
void StartDriving::Update() {
    // Clear all warnings when starting to drive
    status_bar_.ClearAllWarnings();
}
```

## Available LVGL Symbols

You can use these built-in symbols as icons:

- `LV_SYMBOL_WARNING` - Warning triangle
- `LV_SYMBOL_STOP` - Stop sign
- `LV_SYMBOL_CHARGE` - Battery/charging
- `LV_SYMBOL_POWER` - Power/lightning
- `LV_SYMBOL_SETTINGS` - Gear/settings
- `LV_SYMBOL_CLOSE` - X/close
- `LV_SYMBOL_BELL` - Alert bell

Or use any text you want!

## API Reference

```cpp
// Add a warning, returns unique ID
int AddWarning(const char* icon_text);

// Remove a specific warning by ID
void RemoveWarning(int warning_id);

// Remove all warnings
void ClearAllWarnings();

// Check if visible
bool IsVisible() const;

// Manually show/hide (normally auto-managed)
void SetVisible(bool visible);
```

## Visual Design

```
┌─────────────────────────────────────────────┐
│ RED/ORANGE BAR (50px tall)                  │
│ [WARNING] [STOP] TEMP! (icons appear here)  │
├─────────────────────────────────────────────┤
│                                             │
│         Your Screen Content Here           │
│                                             │
│                                             │
└─────────────────────────────────────────────┘
```

- **Color**: `#FF4500` (OrangeRed)
- **Position**: Top of screen
- **Height**: 50 pixels
- **Icons**: White text, Montserrat 24 font
- **Spacing**: 10px between icons

## Implementation Notes

- The status bar is created automatically in `Screen::Create()`
- It's a member of the base `Screen` class: `protected: StatusBar status_bar_;`
- No manual memory management needed - LVGL handles cleanup
- Hidden by default until you add a warning
- Independent of screen transitions - warnings don't persist between screens (each screen has its own instance)

## Example: Temperature Warning on Drive Screen

```cpp
// In DriveModeMenu.hpp
private:
    int temp_warning_id_ = -1;
    
// In DriveModeMenu.cpp
void DriveModeMenu::Update() {
    auto veh_msg = display_->veh_bus.GetRxVehicleStatus();
    
    if (!veh_msg.has_value()) return;
    
    // Monitor motor temperature
    if (veh_msg->MotorTemp() > 85) {
        if (temp_warning_id_ == -1) {  // Not already showing
            temp_warning_id_ = status_bar_.AddWarning(LV_SYMBOL_WARNING " OVERHEAT");
        }
    } else {
        if (temp_warning_id_ != -1) {  // Currently showing
            status_bar_.RemoveWarning(temp_warning_id_);
            temp_warning_id_ = -1;
        }
    }
}
```
