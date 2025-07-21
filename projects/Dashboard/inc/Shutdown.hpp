#pragma once

#include "Screen.hpp"
#include "lvgl.h"

class Shutdown : public Screen {
public:
    Shutdown(Display* display);

    void CreateGUI(void) override;
    void Update(void) override;
};