#include "inc/ButtonHandler.hpp"

#include "../bindings.hpp"

Button::Button(shared::periph::DigitalInput& input) : input_(input) {
    current_state_ = false;
    previous_state_ = false;
    raw_state_ = false;
    last_change_ = 0;
    debounce_time_ = kDebounceDelay;
}

bool Button::PosEdge() const {
    return !previous_state_ && current_state_;
}

bool Button::NegEdge() const {
    return previous_state_ & !current_state_;
}

bool Button::IsPressed() const {
    return current_state_;
}

/***************************************************************
    old
***************************************************************/

ButtonHandler::ButtonHandler(uint32_t debounce_delay) {
    // initialize scroll button
    scroll_.current_state = false;
    scroll_.previous_state = false;
    scroll_.raw_state = false;
    scroll_.last_change = 0;
    scroll_.debounce_time = debounce_delay;

    // initialize select button
    select_.current_state = false;
    select_.previous_state = false;
    select_.raw_state = false;
    select_.last_change = 0;
    select_.debounce_time = debounce_delay;
}

bool ButtonHandler::updateButton(ButtonState& btn, bool raw_input) {
    uint32_t current_time = lv_tick_get();

    btn.raw_state = raw_input;

    // check if enough time has passed since last change
    if (current_time - btn.last_change >= btn.debounce_time) {
        if (btn.raw_state != btn.current_state) {
            btn.previous_state = btn.current_state;
            btn.current_state = btn.raw_state;
            btn.last_change = current_time;
            return true;
        }
    }
    return false;
}

void ButtonHandler::update(lv_indev_drv_t* indev_drv, lv_indev_data_t* data) {
    updateButton(scroll_, bindings::button_scroll.Read());
    updateButton(select_, bindings::button_select.Read());

    // simulate tab and enter keys for scroll and select respectively
    if (scroll_.current_state && !scroll_.previous_state) {
        data->key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PR;
    } else if (select_.current_state && !select_.previous_state) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    // update previous states
    scroll_.previous_state = scroll_.current_state;
    select_.previous_state = select_.current_state;
}

// helper functions
bool ButtonHandler::getScrollState() const {
    return scroll_.current_state;
}

bool ButtonHandler::getSelectState() const {
    return select_.current_state;
}

bool ButtonHandler::isScrollPressed() const {
    return scroll_.current_state && !scroll_.previous_state;
}

bool ButtonHandler::isSelectPressed() const {
    return select_.current_state && !select_.previous_state;
}

bool ButtonHandler::isScrollReleased() const {
    return !scroll_.current_state && scroll_.previous_state;
}

bool ButtonHandler::isSelectReleased() const {
    return !select_.current_state && select_.previous_state;
}