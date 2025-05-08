#pragma once

#include "Screen.hpp"

class StartHV : public Screen {
public:
    StartHV(Display* display);

    void CreateGUI() override;
    void Update() override;
};
