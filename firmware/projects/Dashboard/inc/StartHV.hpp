#pragma once

#include "Screen.hpp"
#include "inc/ButtonHandler.hpp"

class StartHV : public Screen {
public:
    StartHV(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;
};
