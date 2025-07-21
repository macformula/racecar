#pragma once

#include "Screen.hpp"

class StartDriving : public Screen {
public:
    StartDriving(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
};
