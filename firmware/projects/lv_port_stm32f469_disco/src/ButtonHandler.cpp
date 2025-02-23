#include "inc/ButtonHandler.hpp"
#include "../bindings.hpp"

ButtonHandler::ButtonHandler(uint32_t debounce_delay) {
    
    //initialize scroll button
    scroll_.current_state = false;
    scroll_.previous_state = false;
    scroll_.raw_state = false;
    scroll_.last_change = 0;
    scroll_.debounce_time = debounce_delay;
    
    //initialize select button
    select_.current_state = false;
    select_.previous_state = false;
    select_.raw_state = false;
    select_.last_change = 0;
    select_.debounce_time = debounce_delay;
}

bool ButtonHandler::updateButton(ButtonState& btn, bool raw_input) {
    uint32_t current_time = lv_tick_get();
    
    btn.raw_state = raw_input;
    
    //check if enough time has passed since last change
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
    
    //ipdate button states
    updateButton(scroll_, bindings::readScroll());
    updateButton(select_, bindings::readSelect());

    //simulate tab and enter keys for scroll and select respectively
    if (scroll_.current_state && !scroll_.previous_state) {
        data->key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PR;
    }
    else if (select_.current_state && !select_.previous_state) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    //update previous states
    scroll_.previous_state = scroll_.current_state;
    select_.previous_state = select_.current_state;
}

//helper functions
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