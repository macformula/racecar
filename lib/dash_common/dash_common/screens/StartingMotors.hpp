#pragma once

#include "dash_common/Screen.hpp"
#include "lvgl.h"

class StartingMotors : public Screen {
public:
    StartingMotors(Display* display);

    void CreateGUI() override;
    void Update() override;
};