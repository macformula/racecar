#pragma once

#include "Screen.hpp"
#include "inc/ButtonHandler.hpp"

class StartDriving : public Screen {
public:
    StartDriving(Menu* menu);

    void PostCreate() override;
    void Update(Button select, Button scroll) override;

private:
};
