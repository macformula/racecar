#pragma once

#include "dash_common/Screen.hpp"

class StartHV : public Screen {
public:
    StartHV(Display* display);

    void CreateGUI() override;
    void Update() override;
};
