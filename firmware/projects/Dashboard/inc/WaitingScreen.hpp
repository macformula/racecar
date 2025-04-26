#pragma once

#include "Screen.hpp"
#include "inc/ButtonHandler.hpp"

class WaitingScreen : public Screen {
public:
    WaitingScreen(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;
};
