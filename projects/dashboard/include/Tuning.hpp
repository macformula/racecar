#pragma once

#include "inc/Screen.hpp"

class Tuning : public Screen {
public:
    Tuning(Display* display);

    void CreateGUI() override;
    void Update() override;
};
