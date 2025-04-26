#pragma once

#include "Screen.hpp"
#include "inc/ButtonHandler.hpp"
#include "lvgl/lvgl.h"

class ConfirmMenu : public Screen {
    enum Selection : bool {
        CONFIRM = false,
        RESET = true
    };

public:
    ConfirmMenu(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;

private:
    lv_obj_t* roller;
};
