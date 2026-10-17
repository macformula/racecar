#pragma once

#include "Screen.hpp"
#include "lvgl.h"

class ConfirmMenu : public Screen {
    enum Selection : bool {
        CONFIRM = false,
        RESET = true
    };

public:
    ConfirmMenu(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    lv_obj_t* roller;
};
