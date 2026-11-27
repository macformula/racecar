#pragma once

#include "dash_common/Screen.hpp"

class StartDriving : public Screen {
public:
    StartDriving(Display* display);

    void CreateGUI() override;
    void Update() override;

private:
};
