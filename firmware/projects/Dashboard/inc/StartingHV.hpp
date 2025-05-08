#pragma once

#include "Screen.hpp"
#include "lvgl.h"

class StartingHV : public Screen {
public:
    StartingHV(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
    lv_obj_t* progress_bar_;
};