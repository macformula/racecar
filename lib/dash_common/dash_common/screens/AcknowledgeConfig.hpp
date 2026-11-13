#pragma once

#include "dash_common/Screen.hpp"

class AcknowledgeConfig : public Screen {
public:
    AcknowledgeConfig(Display* display);

    void CreateGUI() override;
    void Update() override;
};
