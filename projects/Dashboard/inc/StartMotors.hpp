#pragma once

#include "Screen.hpp"

class StartMotors : public Screen {
public:
    StartMotors(Display* display);

    void CreateGUI() override;
    void Update() override;
};
