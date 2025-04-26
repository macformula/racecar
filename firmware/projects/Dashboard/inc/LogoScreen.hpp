#pragma once

#include "inc/ButtonHandler.hpp"
#include "inc/Screen.hpp"

class LogoScreen : public Screen {
public:
    LogoScreen(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;
};
