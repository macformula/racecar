#pragma once

#include "Screen.hpp"

class ErrorScreen : public Screen {
public:
    ErrorScreen(Display* display);

    void CreateGUI(void) override;
    void Update(void) override;

private:
    lv_obj_t* error_text_;
};