#pragma once

#include "dash_common/Screen.hpp"

class StartMotors : public Screen {
public:
    StartMotors(Display* display);

    void CreateGUI() override;
    void Update() override;
};
