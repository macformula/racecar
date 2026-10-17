#pragma once

#include "Screen.hpp"

class AcknowledgeConfig : public Screen {
public:
    AcknowledgeConfig(Display* display);

    void CreateGUI() override;
    void Update() override;
};
