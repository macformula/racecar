#pragma once

#include "Screen.hpp"
#include "inc/ButtonHandler.hpp"

class StartMotors : public Screen {
public:
    StartMotors(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;
};
