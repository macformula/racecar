#pragma once

#include "inc/Screen.hpp"

class LogoScreen : public Screen {
public:
    LogoScreen(Display* display);

    void CreateGUI() override;
    void Update() override;
};
